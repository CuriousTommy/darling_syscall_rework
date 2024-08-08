.data

.text
.globl _darling_syscall
_darling_syscall:
    movl $0x2000000, %eax
    movq %rcx, %r10         # I don't know why... but this is needed to get indirect syscalls working. TODO: Research why it's needed
    syscall

    jnb 2f
    movq %rax, %rdi
    call _darling_cerror

2:
    ret