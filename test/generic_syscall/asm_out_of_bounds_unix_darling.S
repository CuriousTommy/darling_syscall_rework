// Based on https://stackoverflow.com/a/47836054/5988706

#if defined(__APPLE__)
.text
.globl _darling_asm_out_of_bounds
_darling_asm_out_of_bounds:
#else
.text
.globl darling_asm_out_of_bounds
darling_asm_out_of_bounds:
#endif
    push %rbp
    mov %rsp, %rbp

    movl $0x200FFFF, %eax # Request invalid unix system number
    call ___darling_handle_x86_64_syscall

    jnb 2f
    movq %rax, %rdi
#if defined(__APPLE__)
    call _darling_cerror
#else
    call darling_cerror
#endif

2:
    pop %rbp
    ret