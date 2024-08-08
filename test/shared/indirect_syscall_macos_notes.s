.data

.text
.globl _darling_syscall
_darling_syscall:
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/libsyscall/custom/__syscall.s#L51
    __SYSCALL(___syscall, syscall, 0);
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/libsyscall/custom/SYS.h#L161-L163
#define __SYSCALL(pseudo, name, nargs)			\
	    PSEUDO(pseudo, name, nargs, cerror)			;\
	    ret
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/libsyscall/custom/SYS.h#L153-L155
#define PSEUDO(pseudo, name, nargs, cerror)			\
    LEAF(pseudo, 0)					;\
	    UNIX_SYSCALL_NONAME(name, nargs, cerror)
#  https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/EXTERNAL_HEADERS/architecture/i386/asm_help.h#L161-L165
#define	LEAF(name, localvarsize)			\
    	.globl	name					;\
    	ALIGN						;\
    name:							;\
    	LEAF_FUNCTION_PROLOGUE(localvarsize)
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/EXTERNAL_HEADERS/architecture/i386/asm_help.h#L52-L53
#define ALIGN						\
	.align	2, 0x90
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/libsyscall/custom/SYS.h#L144-L151
#define UNIX_SYSCALL_NONAME(name, nargs, cerror)		 \
	    .globl	cerror								;\
	    movl	$ SYSCALL_CONSTRUCT_UNIX(SYS_##name), %eax			;\
	    UNIX_SYSCALL_SYSCALL							;\
	    jnb		2f							;\
	    movq	%rax, %rdi							;\
	    BRANCH_EXTERN(_##cerror)						;\
    2:
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/mach/i386/syscall_sw.h#L158-L160
#define SYSCALL_CONSTRUCT_UNIX(syscall_number) \
			((SYSCALL_CLASS_UNIX << SYSCALL_CLASS_SHIFT) | \
			 (SYSCALL_NUMBER_MASK & (syscall_number)))
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/libsyscall/custom/SYS.h#L130-L132
#define UNIX_SYSCALL_SYSCALL	\
	    movq	%rcx, %r10		;\
	    syscall
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/EXTERNAL_HEADERS/architecture/i386/asm_help.h#L292-L295
#define BRANCH_EXTERN(func)	\
	    PICIFY(func)		; \
	    jmp	%edx		; \
	    NON_LAZY_STUB(func)
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/EXTERNAL_HEADERS/architecture/i386/asm_help.h#L271-L275
#define PICIFY(var)					\
	    call	1f					; \
    1:							; \
	    popl	%edx					; \
	    movl	L ## var ## $non_lazy_ptr-1b(%edx),%edx
# https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/EXTERNAL_HEADERS/architecture/i386/asm_help.h#L281-L286
#define NON_LAZY_STUB(var)	\
    .non_lazy_symbol_pointer	; \
    L ## var ## $non_lazy_ptr:	; \
    .indirect_symbol var		; \
    .long 0				; \
    .text