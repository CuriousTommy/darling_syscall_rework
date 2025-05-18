// Workaround due the lack of ARM64 & WSL1 Support (through lxmonika) 
// for User Dispatch Syscall

#ifdef __x86_64__

.globl ___darling_handle_x86_64_syscall
___darling_handle_x86_64_syscall:
    // Preserve stack and frame pointer
    push %rbp
    mov %rsp, %rbp

    // preserve temp registers (see `darling_register_x86_64_t` for layout)
    push %rsp
    push %rbp
    pushfq
    push %r11
    push %r10
    push %r9
    push %r8
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rax

    // Call _darling_handle_x86_64_syscall(stack_pointer);
    mov %rsp, %rdi
#if defined(__APPLE__)
    call __darling_handle_x86_64_syscall
#else
    call _darling_handle_x86_64_syscall
#endif

    // Restore temporary registers (includes updated return values)
    pop %rax
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %r8
    pop %r9
    pop %r10
    pop %r11
    popfq
    pop %rbp
    pop %rsp

    // Restore frame pointer and return
    pop %rbp
    ret

#else
#error "Missing ASM Syscall Bridge"
#endif