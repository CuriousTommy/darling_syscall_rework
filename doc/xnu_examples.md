# XNU Examples

## Hander Examples

[ARM64 SVC Handler](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/arm64/sleh.c#L1624-L1669):
```c
static void
handle_svc(arm_saved_state_t *state)
{
	int      trap_no = get_saved_state_svc_number(state);
	thread_t thread  = current_thread();
	struct   proc *p;

	//...

	if (trap_no == (int)PLATFORM_SYSCALL_TRAP_NO) {
		platform_syscall(state);
		panic("Returned from platform_syscall()?");
	}

	// ...
	
	if (trap_no < 0) {
		switch (trap_no) {
		case MACH_ARM_TRAP_ABSTIME:
			handle_mach_absolute_time_trap(state);
			return;
		case MACH_ARM_TRAP_CONTTIME:
			handle_mach_continuous_time_trap(state);
			return;
		}

		// ...
		mach_syscall(state);
	} else {
		// ...
		p = get_bsdthreadtask_info(thread);
		// ...
		unix_syscall(state, thread, (struct uthread*)thread->uthread, p);
	}
}
```

[i386 Syscall Dispatch Routines](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/x86_64/idt64.s#L1790-L1863)
```assembly
/*
 *
 * 32bit Tasks
 * System call entries via INTR_GATE or sysenter:
 *
 *	r15	 x86_saved_state32_t
 *	rsp	 kernel stack
 *
 *	both rsp and r15 are 16-byte aligned
 *	interrupts disabled
 *	direction flag cleared
 */

Entry(hndl_sysenter)
	/*
	 * We can be here either for a mach syscall or a unix syscall,
	 * as indicated by the sign of the code:
	 */
	movl	R32_EAX(%r15),%eax
	testl	%eax,%eax
	js	EXT(hndl_mach_scall)		/* < 0 => mach */
						/* > 0 => unix */
	
Entry(hndl_unix_scall)
    TIME_TRAP_UENTRY
	movq	%gs:CPU_ACTIVE_THREAD,%rcx	/* get current thread     */
	movq	TH_TASK(%rcx),%rbx		/* point to current task  */
	incl	TH_SYSCALLS_UNIX(%rcx)		/* increment call count   */
	/* Check for active vtimers in the current task */
	TASK_VTIMER_CHECK(%rbx,%rcx)
	sti
	CCALL1(unix_syscall, %r15)
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_mach_scall)
	TIME_TRAP_UENTRY
	movq	%gs:CPU_ACTIVE_THREAD,%rcx	/* get current thread     */
	movq	TH_TASK(%rcx),%rbx		/* point to current task  */
	incl	TH_SYSCALLS_MACH(%rcx)		/* increment call count   */
	/* Check for active vtimers in the current task */
	TASK_VTIMER_CHECK(%rbx,%rcx)
	sti
	CCALL1(mach_call_munger, %r15)
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_mdep_scall)
	TIME_TRAP_UENTRY
	/* Check for active vtimers in the current task */
	movq	%gs:CPU_ACTIVE_THREAD,%rcx	/* get current thread     */
	movq	TH_TASK(%rcx),%rbx		/* point to current task  */
	TASK_VTIMER_CHECK(%rbx,%rcx)
	sti
	CCALL1(machdep_syscall, %r15)
	/*
	 * always returns through thread_exception_return
	 */
```

[x86-64 Syscall Dispatch Routines](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/x86_64/idt64.s#L1865-L1946C17):
```assembly
/*
 * 64bit Tasks
 * System call entries via syscall only:
 *
 *	r15	 x86_saved_state64_t
 *	rsp	 kernel stack
 *
 *	both rsp and r15 are 16-byte aligned
 *	interrupts disabled
 *	direction flag cleared
 */


Entry(hndl_syscall)
	TIME_TRAP_UENTRY
	movq	%gs:CPU_ACTIVE_THREAD,%rcx	/* get current thread     */
	movl	$-1, TH_IOTIER_OVERRIDE(%rcx)	/* Reset IO tier override to -1 before handling syscall */
	movq	TH_TASK(%rcx),%rbx		/* point to current task  */

	/* Check for active vtimers in the current task */
	TASK_VTIMER_CHECK(%rbx,%rcx)

	/*
	 * We can be here either for a mach, unix machdep or diag syscall,
	 * as indicated by the syscall class:
	 */
	movl	R64_RAX(%r15), %eax		/* syscall number/class */
	movl	%eax, %edx
	andl	$(SYSCALL_CLASS_MASK), %edx	/* syscall class */
	cmpl	$(SYSCALL_CLASS_MACH<<SYSCALL_CLASS_SHIFT), %edx
	je	EXT(hndl_mach_scall64)
	cmpl	$(SYSCALL_CLASS_UNIX<<SYSCALL_CLASS_SHIFT), %edx
	je	EXT(hndl_unix_scall64)
	cmpl	$(SYSCALL_CLASS_MDEP<<SYSCALL_CLASS_SHIFT), %edx
	je	EXT(hndl_mdep_scall64)
	cmpl	$(SYSCALL_CLASS_DIAG<<SYSCALL_CLASS_SHIFT), %edx
	je	EXT(hndl_diag_scall64)

	/* Syscall class unknown */
	sti
	CCALL3(i386_exception, $(EXC_SYSCALL), %rax, $1)
	/* no return */

Entry(hndl_unix_scall64)
	incl	TH_SYSCALLS_UNIX(%rcx)		/* increment call count   */
	sti
	CCALL1(unix_syscall64, %r15)
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_mach_scall64)
	incl	TH_SYSCALLS_MACH(%rcx)		/* increment call count   */
	sti
	CCALL1(mach_call_munger64, %r15)
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_mdep_scall64)
	sti
	CCALL1(machdep_syscall64, %r15)
	/*
	 * always returns through thread_exception_return
	 */

Entry(hndl_diag_scall64)
	CCALL1(diagCall64, %r15)	// Call diagnostics
	test	%eax, %eax		// What kind of return is this?
	je	1f			// - branch if bad (zero)
	jmp	EXT(return_to_user)	// Normal return, do not check asts...
1:
	sti
	CCALL3(i386_exception, $EXC_SYSCALL, $0x6000, $1)
	/* no return */
```

## Unix Syscall

[`uthread` struct](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/sys/user.h#L116-L119):
```c
struct uthread {
	/* syscall parameters, results and catches */
	u_int64_t uu_arg[8]; /* arguments to current system call */
	int uu_rval[2];
	// ...
}
```

[osfmk/mach/i386/syscall_sw.h (i386 UNIX_SYSCALL_TRAP)](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/mach/i386/syscall_sw.h#L66-L80)
```assembly
/*
 * Software interrupt codes for 32-bit system call entry:
 */
#define UNIX_INT     0x80

#if defined(__i386__)

/*
 * Syscall entry macros for use in libc:
 */
#define UNIX_SYSCALL_TRAP	\
	int $(UNIX_INT)
```

[osfmk/mach/i386/syscall_sw.h (x86-64 UNIX_SYSCALL_TRAP)](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/mach/i386/syscall_sw.h#L96-L103)
```assembly
#if defined(__x86_64__)
#define UNIX_SYSCALL_TRAP	\
	syscall
```

[(bsd/dev/i386/systemcalls.c) unix_syscall64](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/dev/i386/systemcalls.c#L287-L504)
```c
__attribute__((noreturn))
void
unix_syscall64(x86_saved_state_t *state)
{
	void                    *vt;
	unsigned int    code, syscode;
	const struct sysent *callp;
	int             args_in_regs;
	boolean_t       args_start_at_rdi;
	int             error;
	struct proc     *p;
	x86_saved_state64_t *regs;

	// ...
	regs = saved_state64(state);
	// ...
	code    = regs->rax & SYSCALL_NUMBER_MASK;
	syscode = (code < nsysent) ? code : SYS_invalid;
	callp = &sysent[syscode];

	vt = (void *)uthread->uu_arg;

	// ...
	if (__improbable(callp == sysent)) {
		/*
		 * indirect system call... system call number
		 * passed as 'arg0'
		 */
		code    = regs->rdi;
		syscode = (code < nsysent) ? code : SYS_invalid;
		callp   = &sysent[syscode];
		args_start_at_rdi = FALSE;
		args_in_regs = 5;
	} else {
		args_start_at_rdi = TRUE;
		args_in_regs = 6;
	}

	if (callp->sy_narg != 0) {
		assert(callp->sy_narg <= 8); /* size of uu_arg */

		args_in_regs = MIN(args_in_regs, callp->sy_narg);
		memcpy(vt, args_start_at_rdi ? &regs->rdi : &regs->rsi, args_in_regs * sizeof(syscall_arg_t));

		if (__improbable(callp->sy_narg > args_in_regs)) {
			int copyin_count;
			copyin_count = (callp->sy_narg - args_in_regs) * sizeof(syscall_arg_t);

			error = copyin((user_addr_t)(regs->isf.rsp + sizeof(user_addr_t)), (char *)&uthread->uu_arg[args_in_regs], copyin_count);
			if (error) {
				regs->rax = error;
				regs->isf.rflags |= EFL_CF;
				thread_exception_return();
				/* NOTREACHED */
			}
		}
	}

	// ...
	uthread->uu_rval[0] = 0;
	uthread->uu_rval[1] = 0;
	uthread->uu_flag |= UT_NOTCANCELPT;
	uthread->syscall_code = code;

	error = (*(callp->sy_call))((void *) p, vt, &(uthread->uu_rval[0]));

	// ...
	if (__improbable(error == ERESTART)) {
		/*
		 * all system calls come through via the syscall instruction
		 * in 64 bit mode... its 2 bytes in length
		 * move the user's pc back to repeat the syscall:
		 */
		pal_syscall_restart( thread, state );
	} else if (error != EJUSTRETURN) {
		if (__improbable(error)) {
			regs->rax = error;
			regs->isf.rflags |= EFL_CF;     /* carry bit */
		} else { /* (not error) */
			switch (callp->sy_return_type) {
			case _SYSCALL_RET_INT_T:
				regs->rax = uthread->uu_rval[0];
				regs->rdx = uthread->uu_rval[1];
				break;
			case _SYSCALL_RET_UINT_T:
				regs->rax = ((u_int)uthread->uu_rval[0]);
				regs->rdx = ((u_int)uthread->uu_rval[1]);
				break;
			case _SYSCALL_RET_OFF_T:
			case _SYSCALL_RET_ADDR_T:
			case _SYSCALL_RET_SIZE_T:
			case _SYSCALL_RET_SSIZE_T:
			case _SYSCALL_RET_UINT64_T:
				regs->rax = *((uint64_t *)(&uthread->uu_rval[0]));
				regs->rdx = 0;
				break;
			case _SYSCALL_RET_NONE:
				break;
			default:
				panic("unix_syscall: unknown return type");
				break;
			}
			regs->isf.rflags &= ~EFL_CF;
		}
	}
	
	// ...
	uthread->uu_flag &= ~UT_NOTCANCELPT;
	uthread->syscall_code = 0;
}
```

### Mach
[(osfmk/i386/bsd_i386.c) mach_call_args](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L452-L462)
```c
struct mach_call_args {
	syscall_arg_t arg1;
	syscall_arg_t arg2;
	syscall_arg_t arg3;
	syscall_arg_t arg4;
	syscall_arg_t arg5;
	syscall_arg_t arg6;
	syscall_arg_t arg7;
	syscall_arg_t arg8;
	syscall_arg_t arg9;
};
```

[(osfmk/i386/bsd_i386.c) mach_call_munger64](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L611-L727)
```c
__attribute__((noreturn))
void
mach_call_munger64(x86_saved_state_t *state)
{
	int call_number;
	int argc;
	mach_call_t mach_call;
	struct mach_call_args args = {
		.arg1 = 0,
		.arg2 = 0,
		.arg3 = 0,
		.arg4 = 0,
		.arg5 = 0,
		.arg6 = 0,
		.arg7 = 0,
		.arg8 = 0,
		.arg9 = 0
	};
	x86_saved_state64_t     *regs;

	// ...

	regs = saved_state64(state);

	call_number = (int)(regs->rax & SYSCALL_NUMBER_MASK);

	// ...

	if (call_number < 0 || call_number >= mach_trap_count) {
		i386_exception(EXC_SYSCALL, regs->rax, 1);
		/* NOTREACHED */
	}
	mach_call = (mach_call_t)mach_trap_table[call_number].mach_trap_function;

	if (mach_call == (mach_call_t)kern_invalid) {
		i386_exception(EXC_SYSCALL, regs->rax, 1);
		/* NOTREACHED */
	}
	argc = mach_trap_table[call_number].mach_trap_arg_count;
	if (argc) {
		int args_in_regs = MIN(6, argc);
		__nochk_memcpy(&args.arg1, &regs->rdi, args_in_regs * sizeof(syscall_arg_t));

		if (argc > 6) {
			int copyin_count;

			assert(argc <= 9);
			copyin_count = (argc - 6) * (int)sizeof(syscall_arg_t);

			if (copyin((user_addr_t)(regs->isf.rsp + sizeof(user_addr_t)), (char *)&args.arg7, copyin_count)) {
				regs->rax = KERN_INVALID_ARGUMENT;
				thread_exception_return();
				/* NOTREACHED */
			}
		}
	}

	// ...

	regs->rax = (uint64_t)mach_call((void *)&args);

	// ...

	thread_exception_return();
	/* NOTREACHED */
}
```

### Machdep

[(osfmk/i386/machdep_call.h) machdep_call_t](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/machdep_call.h#L39-L71)
```c
typedef union {
	kern_return_t           (*args_0)(void);
	kern_return_t           (*args_1)(uint32_t);
	kern_return_t           (*args64_1)(uint64_t);
	kern_return_t           (*args_2)(uint32_t, uint32_t);
	kern_return_t           (*args64_2)(uint64_t, uint64_t);
	kern_return_t           (*args_3)(uint32_t, uint32_t, uint32_t);
	kern_return_t           (*args64_3)(uint64_t, uint64_t, uint64_t);
	kern_return_t           (*args_4)(uint32_t, uint32_t, uint32_t, uint32_t);
	kern_return_t           (*args_var)(uint32_t, ...);
	int                     (*args_bsd_3)(uint32_t *, uint32_t,
	    uint32_t, uint32_t);
	int                     (*args64_bsd_3)(uint32_t *, uint64_t,
	    uint64_t, uint64_t);
} machdep_call_routine_t;

typedef struct {
	machdep_call_routine_t      routine;
	int                         nargs;
	int                         bsd_style;
} machdep_call_t;
```

[osfmk/mach/i386/syscall_sw.h (i386 UNIX_SYSCALL_TRAP)](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/mach/i386/syscall_sw.h#L66-L80)
```assembly
/*
 * Software interrupt codes for 32-bit system call entry:
 */
#define MACHDEP_INT  0x82

#if defined(__i386__)

/*
 * Syscall entry macros for use in libc:
 */
#define MACHDEP_SYSCALL_TRAP	\
	int $(MACHDEP_INT)
```

[osfmk/mach/i386/syscall_sw.h (x86-64 UNIX_SYSCALL_TRAP)](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/mach/i386/syscall_sw.h#L96-L103)
```assembly
#if defined(__x86_64__)
#define MACHDEP_SYSCALL_TRAP	\
	syscall
```

[(osfmk/i386/machdep_call.c) machdep_call_table64](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/machdep_call.c#L54-L67)
```c
const machdep_call_t            machdep_call_table64[] = {
#if HYPERVISOR
	MACHDEP_CALL_ROUTINE64(hv_task_trap, 2),
	MACHDEP_CALL_ROUTINE64(hv_thread_trap, 2),
#else
	MACHDEP_CALL_ROUTINE(kern_invalid, 0),
	MACHDEP_CALL_ROUTINE(kern_invalid, 0),
#endif
	MACHDEP_CALL_ROUTINE(kern_invalid, 0),
	MACHDEP_CALL_ROUTINE64(thread_fast_set_cthread_self64, 1),
	MACHDEP_CALL_ROUTINE(kern_invalid, 0),
	MACHDEP_BSD_CALL_ROUTINE64(i386_set_ldt64, 3),
	MACHDEP_BSD_CALL_ROUTINE64(i386_get_ldt64, 3)
};
```

[(osfmk/i386/machdep_call.h) MACHDEP_CALL_ROUTINE64](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/machdep_call.h#L58-L59)
```c
#define MACHDEP_CALL_ROUTINE64(func, args)      \
	{ { .args64_ ## args = func }, args, 0 }
```

[(osfmk/i386/bsd_i386.c) machdep_syscall64](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L378-L445)
```c
__attribute__((noreturn))
void
machdep_syscall64(x86_saved_state_t *state)
{
	int                     trapno;
	const machdep_call_t    *entry;
	x86_saved_state64_t     *regs;

	assert(is_saved_state64(state));
	regs = saved_state64(state);

	trapno = (int)(regs->rax & SYSCALL_NUMBER_MASK);

	// ...

	if (trapno < 0 || trapno >= machdep_call_count) {
		regs->rax = (unsigned int)kern_invalid(NULL);

		thread_exception_return();
		/* NOTREACHED */
	}
	entry = &machdep_call_table64[trapno];

	switch (entry->nargs) {
	case 0:
		regs->rax = (*entry->routine.args_0)();
		break;
	case 1:
		regs->rax = (*entry->routine.args64_1)(regs->rdi);
		break;
	case 2:
		regs->rax = (*entry->routine.args64_2)(regs->rdi, regs->rsi);
		break;
	case 3:
		if (!entry->bsd_style) {
			regs->rax = (*entry->routine.args64_3)(regs->rdi, regs->rsi, regs->rdx);
		} else {
			int             error;
			uint32_t        rval;


			error = (*entry->routine.args64_bsd_3)(&rval, regs->rdi, regs->rsi, regs->rdx);
			if (error) {
				regs->rax = (uint64_t)error;
				regs->isf.rflags |= EFL_CF;    /* carry bit */
			} else {
				regs->rax = rval;
				regs->isf.rflags &= ~(uint64_t)EFL_CF;
			}
		}
		break;
	default:
		panic("machdep_syscall64: too many args");
	}

	// ...

	thread_exception_return();
	/* NOTREACHED */
}
```
## Syscall Examples
### Unix

[`fork` syscall](https://github.com/darlinghq/darling-xnu/blob/814a90cae0b725c65b89dcba2b680d36aceeeec4/bsd/kern/kern_fork.c#L906-L951):
* This syscall sets two return values to the `retval` (`retval[0]` & `retval[1]`)
* The error code is returned using the normal C-style `return`

```c
int
fork(proc_t parent_proc, __unused struct fork_args *uap, int32_t *retval)
{
	thread_t child_thread;
	int err;

	retval[1] = 0;          /* flag parent return for user space */


	if ((err = fork1(parent_proc, &child_thread, PROC_CREATE_FORK, NULL)) == 0) {
		task_t child_task;
		proc_t child_proc;

		// ...
		retval[0] = child_proc->p_pid;
		// ...

	return err;
}
```

[`write_nocancel` syscall](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/kern/sys_generic.c#L495-L523):
* Arguments are stored in a special struct (generated?)

```c
int
write_nocancel(struct proc *p, struct write_nocancel_args *uap, user_ssize_t *retval)
{
	struct fileproc *fp;
	int error;
	int fd = uap->fd;

	// ...

	error = fp_lookup(p, fd, &fp, 0);
	if (error) {
		return error;
	}
	if ((fp->f_flag & FWRITE) == 0) {
		error = EBADF;
	} else if (fp_isguarded(fp, GUARD_WRITE)) {
		// ...
		error = fp_guard_exception(p, fd, fp, kGUARD_EXC_WRITE);
		// ...
	} else {
		// ...
		error = dofilewrite(&context, fp, uap->cbuf, uap->nbyte,
		    (off_t)-1, 0, retval);
	}
	// ...
	return error;
}
```
### Machdep

[(osfmk/i386/bsd_i386_native.c) thread_fast_set_cthread_self64](https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386_native.c#L164-L178)
```c
/*
 * thread_fast_set_cthread_self64: Sets the machine kernel thread ID of the
 * current thread to the given thread ID; fast version for 64-bit processes
 *
 * Parameters:    self                    Thread ID
 *
 * Returns:        0                      Success
 *                !0                      Not success
 */
kern_return_t
thread_fast_set_cthread_self64(uint64_t self)
{
	machine_thread_set_tsd_base(current_thread(), self);
	return USER_CTHREAD; /* N.B.: not a kern_return_t! */
}
```