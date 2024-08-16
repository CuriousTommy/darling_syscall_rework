.data

.text
#if defined(__APPLE__)
.globl _main
_main:
#else
.globl main
main:
#endif
    push %rbp
    mov %rsp, %rbp

    mov $1, %rax
    mov $2, %rdi
    mov $3, %rsi
    mov $4, %rdx
    mov $5, %rcx
    mov $6, %r8 
    mov $7, %r9 
    mov $8, %r10
    mov $9, %r11
    call ___darling_handle_x86_64_syscall

    pop %rbp
    ret