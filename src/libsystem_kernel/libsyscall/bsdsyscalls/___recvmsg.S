#define __SYSCALL_32BIT_ARG_BYTES 12
#include "SYS.h"

#ifndef SYS_recvmsg
#error "SYS_recvmsg not defined. The header files libsyscall is building against do not match syscalls.master."
#endif

#if defined(__arm64__)
MI_ENTRY_POINT(___recvmsg)
SYSCALL_NONAME(recvmsg, 3, cerror)
ret
#else
#if defined(__arm64__) || defined(__i386__) || defined(__x86_64__) || defined(__ppc__)
__SYSCALL2(___darling_recvmsg, recvmsg, 3, cerror)
#endif

#endif

#if defined(__arm64__)
	.globl	_recvmsg
	.set	_recvmsg, ___recvmsg
#endif

#if defined(__i386__)
	.globl	_recvmsg$UNIX2003
	.set	_recvmsg$UNIX2003, ___recvmsg
#endif

#if defined(__x86_64__)
	.globl	_darling_recvmsg
	.set	_darling_recvmsg, ___darling_recvmsg
#endif

#if defined(__ppc__)
	.globl	_recvmsg$UNIX2003
	.set	_recvmsg$UNIX2003, ___recvmsg
#endif

