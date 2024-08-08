// Based on https://stackoverflow.com/a/47836054/5988706

.text
.globl _darling_write
_darling_write:
    push %rbp
    mov %rsp, %rbp

    movl $0x2000004, %eax # preparing unix system call 4
    syscall

    jnb 2f
    movq %rax, %rdi
    call _darling_cerror

2:
    pop %rbp
    ret