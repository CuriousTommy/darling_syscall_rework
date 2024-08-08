# Motivation

Darling's current implementation of handling macOS syscalls requires us to write out custom assembly that grabs the syscall table (ex: `__bsd_syscall_table`) and executes the function that implements the syscall.

```assembly
#if defined(__x86_64__)

__darling_bsd_syscall:
Lentry_hook:
	.space 13, 0x90
	movq    ___bsd_syscall_table@GOTPCREL(%rip), %r10
	movq	(%r10,%rax,8), %r10
	test	%r10, %r10
	jz		.no_sys
	# Re-push possible 7th and 8th arg
	movq	24(%rsp), %r11
	pushq	%r11
	movq	24(%rsp), %r11
	pushq	%r11
	call	*%r10
	addq	$16, %rsp
.std_ret:
Lexit_hook:
	.space 13, 0x90
	ret
.no_sys:
	movq	%rax, %rdi
	call	___unknown_syscall
	jmp		.std_ret

.section        __DATA,__data
.globl __darling_bsd_syscall_entry
.globl __darling_bsd_syscall_exit
__darling_bsd_syscall_entry:
	.quad Lentry_hook
__darling_bsd_syscall_exit:
	.quad Lexit_hook

#elif ...
```

```c
void* __bsd_syscall_table[600] = {
	[0] = sys_syscall,
	[1] = sys_exit,
	[2] = sys_fork,
	[3] = sys_read,
	// ...
};
```

However, this implementation has some issues:

1. The way we implement returning values differs from how Apple handles returning the values.

For example, let's take a look how we handle  `___fork`. Besides the usual changes of using `call __darling_bsd_syscall` instead of `syscall`, we also have to modify how we deal with the return value.

```
LEAF(___fork, 0)
	subq  $24, %rsp   // Align the stack, plus room for local storage


#ifdef DARLING
	movl	$ SYS_fork, %eax
	call	__darling_bsd_syscall
	cmpq	$0, %rax
	jnb L1
#else
	movl 	$ SYSCALL_CONSTRUCT_UNIX(SYS_fork),%eax; // code for fork -> rax
	UNIX_SYSCALL_TRAP		// do the system call
	jnc	L1			// jump if CF==0
#endif


	movq	%rax, %rdi
	CALL_EXTERN(_cerror)
	movq	$-1, %rax
	addq	$24, %rsp   // restore the stack
	ret
	
L1:
#ifdef DARLING
	testl	%eax, %eax
	jnz	L2
#else
	orl	%edx,%edx	// CF=OF=0,  ZF set if zero result	
	jz	L2		// parent, since r1 == 0 in parent, 1 in child
#endif


	//child here...
	xorq	%rax, %rax
	PICIFY(__current_pid)
	movl	%eax,(%r11)
L2:
	// parent ends up here skipping child portion
	addq	$24, %rsp   // restore the stack
	ret
```

2. The current implement would need a major redesign if we want to Linux's syscall user dispatch feature in the distant future.

**Note:** There are other reasons as to why Darling can't take advantage of this feature, for now.

Refer to the following link for details:
https://github.com/darlinghq/darling/issues/944#issuecomment-1939066175

3. The way we setup `xtrace` (`__darling_bsd_syscall_entry` & `__darling_bsd_syscall_exit`) does not work well on ARM64.

the way `xtrace` works in Darling is that we overwrite the `nop` instruction with code that calls the `xtrace` logging.

```assembly
__darling_bsd_syscall:
Lentry_hook:
	.space 13, 0x90
// ...
Lexit_hook:
	.space 13, 0x90
	ret

.section        __DATA,__data
.globl __darling_bsd_syscall_entry
.globl __darling_bsd_syscall_exit
__darling_bsd_syscall_entry:
	.quad Lentry_hook
__darling_bsd_syscall_exit:
	.quad Lexit_hook
```

```c
static void xtrace_setup_bsd(void)
{
	uintptr_t area = (uintptr_t)_darling_bsd_syscall_entry;
	uintptr_t areaEnd = ((uintptr_t)_darling_bsd_syscall_exit) + sizeof(struct hook);

	// __asm__("int3");
	area &= ~(4096-1);
	areaEnd &= ~(4096-1);

	uintptr_t bytes = 4096 + (areaEnd-area);

	mprotect((void*) area, bytes, PROT_READ | PROT_WRITE | PROT_EXEC);

	setup_hook(_darling_bsd_syscall_entry, (void*)darling_bsd_syscall_entry_trampoline, false);
	setup_hook(_darling_bsd_syscall_exit, (void*)darling_bsd_syscall_exit_trampoline, false);

	mprotect((void*) area, bytes, PROT_READ | PROT_EXEC);
}
```

Unfortunately, trying to implement `__darling_mach_syscall_entry` and `__darling_bsd_syscall_exit` on ARM64 (in the same way that is done on x86-64), results in a linker error.

```
ld: chained binds not implemented yet in '__darling_mach_syscall_entry' from ../xnu/darling/src/libsystem_kernel/libsystem_kernel_static64.a(darling_mach_syscall.S.o) for architecture arm64
clang++: error: linker command failed with exit code 1 (use -v to see invocation)
```

4. Syscalls (with some exceptions) are not suppose to overwrite other registers. Some of Apple's assembly code may rely on temporary registers. 

For example, Let's look at ARM64's implementation of `___vfork`: 
https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/libsyscall/custom/__vfork.s#L198-L235
```assembly
#elif defined(__arm64__)

	// ...

	MI_GET_ADDRESS(x9, __current_pid)
Ltry_set_vfork:
	ldxr	w10, [x9]			// Get old current pid value (exclusive)
	mov		w11, #-1			// Will be -1 if current value is positive
	subs	w10, w10, #1		// Subtract one
	csel	w12, w11, w10, pl	// If >= 0, set to -1, else set to (current - 1)
	stxr	w13, w12, [x9]		// Attempt exclusive store to current pid
	cbnz	w13, Ltry_set_vfork	// If store failed, retry
	
	// ...

	// Error case
Lbotch:
	// ...
	MI_GET_ADDRESS(x9, __current_pid) // Reload current pid address
	POP_FRAME
	// Fall through	
Lparent:
	ldxr	w10, [x9]			// Exclusive load current pid value
	add		w10, w10, #1		// Increment (i.e. decrement vfork count)
	stxr	w11, w10, [x9]		// Attempt exclusive store of updated vfork count
	cbnz	w11, Lparent		// If exclusive store failed, retry
	ARM64_STACK_EPILOG		// Done, return
```
Register `x9` through `x13` are temporary registers. With the current implementation, we would have to look through all of Apple's assembly code and add workarounds to perserve those registers.