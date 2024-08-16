#define __SYSCALL_32BIT_ARG_BYTES 12
#include "SYS.h"

#ifndef SYS_write
#error "SYS_write not defined. The header files libsyscall is building against do not match syscalls.master."
#endif

#if defined(__arm64__)
MI_ENTRY_POINT(_darling_write)
ZERO_EXTEND(2)
SYSCALL_NONAME(write, 3, darling_cerror)
ret
#else
#if defined(__arm64__) || defined(__x86_64__)
#if defined(__APPLE__)
__SYSCALL2(_darling_write, write, 3, darling_cerror)
#else
__SYSCALL2(darling_write, write, 3, darling_cerror)
#endif
#else
__SYSCALL2(___darling_write, write, 3, darling_cerror)
#endif

#endif

#if defined(__i386__)
	.globl	_darling_write$UNIX2003
	.set	_darling_write$UNIX2003, ___darling_write
#endif

#if defined(__ppc__)
	.globl	_darling_write$UNIX2003
	.set	_darling_write$UNIX2003, ___darling_write
#endif

