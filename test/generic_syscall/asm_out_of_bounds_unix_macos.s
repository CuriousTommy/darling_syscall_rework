// Based on https://stackoverflow.com/a/47836054/5988706

.text
.globl _darling_asm_out_of_bounds
_darling_asm_out_of_bounds:
    push %rbp
    mov %rsp, %rbp

    movl $0x200FFFF, %eax # Request invalid unix system number
    syscall

    jnb 2f
    movq %rax, %rdi
    call _darling_cerror

2:
    pop %rbp
    ret