#include "recvfrom.h"
#include "../base.h"
#include "../errno.h"
#include <stddef.h>
#include "../bsdthread/cancelable.h"

#if defined(__linux__)
#include <linux-syscalls/linux.h>
#include "socket.h"
#include "duct.h"
#endif

#if defined(__APPLE__)
#include <sys/socket.h>
#include <errno.h>
#endif

extern void *memcpy(void *dest, const void *src, __SIZE_TYPE__ n);

// XNU Implementation: [xnu]/bsd/kern/uipc_syscalls.c
int sys_recvfrom(const struct darling_syscall_args* args, struct darling_syscall_retarg* retargs)
{
	CANCELATION_POINT();
	return sys_recvfrom_nocancel(args, retargs);
}

int sys_recvfrom_nocancel(const struct darling_syscall_args* args, struct darling_syscall_retarg* retargs)
{
	retargs->result_type = DARLING_SYSCALL_RETARG_TYPE_INT_T;
	int* ret = &retargs->result[0];

	int fd = (int)args->arg[0];
	void* buf = (void*)args->arg[1];
	unsigned long len = (unsigned long)args->arg[2];
	int flags = (int)args->arg[3];
	void* from = (void*)args->arg[4];
	int* socklen = (int*)args->arg[5];
	
#if defined(__linux__)
	// TODO: Fix for linux build

	int ret, linux_flags;
	struct sockaddr_fixup* fixed;

	linux_flags = msgflags_bsd_to_linux(flags);

#ifdef __NR_socketcall
	ret = LINUX_SYSCALL(__NR_socketcall, LINUX_SYS_RECVFROM, ((long[6]) { fd, buf, len,
			flags, from, socklen }));
#else
	ret = LINUX_SYSCALL(__NR_recvfrom, fd, buf, len, flags, from, socklen);
#endif

	if (ret < 0)
		ret = errno_linux_to_bsd(ret);
	else if (from != NULL)
	{
		fixed = (struct sockaddr_fixup*) from;
		if ((*socklen = sockaddr_fixup_from_linux(fixed, from, *socklen)) < 0)
			ret = *socklen;
	}

	return ret;

#elif defined(__APPLE__)
	ssize_t result = recvfrom(fd, buf, (size_t)len, flags, (struct sockaddr*)from, (socklen_t*)socklen);
	if (result < 0) {
		return errno;
	}

	*ret = result;
	return 0;
#endif
}

