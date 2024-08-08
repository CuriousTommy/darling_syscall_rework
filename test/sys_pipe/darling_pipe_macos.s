// Based on https://stackoverflow.com/a/47836054/5988706

.text
.globl _darling_pipe
_darling_pipe:
    movl    $0x200002A, %eax # preparing unix system call 42
    syscall
    jnb     2f
    movq    %rax, %rdi

    push    %rbp
    mov     %rsp, %rbp
    movq    %rax, %rdi
    call    _darling_cerror
    pop     %rbp

2:
	movl	%eax, (%rdi)
	movl	%edx, 4(%rdi)
	xorl	%eax, %eax
	ret