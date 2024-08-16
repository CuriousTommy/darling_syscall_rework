#include "write.h"

#include "../base.h"
#include "../errno.h"
#include "../linux-syscalls/linux.h"
#include "../bsdthread/cancelable.h"
#ifdef __APPLE__
#include <unistd.h>
#endif


int sys_write(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs)
{
	CANCELATION_POINT();
	return sys_write_nocancel(args, retargs);
}


int sys_write_nocancel(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs)
{
	retargs->result_type = DARLING_SYSCALL_RETARG_TYPE_SSIZE_T;
	darling_syscall_ssize_t ret;

	int fd = (int)args->arg[0];
	const void* mem = (void*)args->arg[1];
	int len = (int)args->arg[2];

#if defined(__linux__)
	// TODO: Verify for linux build
	ret = LINUX_SYSCALL3(__NR_write, fd, mem, len);
	if (ret < 0)
		return errno_linux_to_bsd(ret);
#elif defined(__APPLE__)	
	ret = write(fd,mem,len);
	if (ret < 0) {
		return errno;
	}
#else
#error "Missing implementation"
#endif

	*((darling_syscall_ssize_t*)&retargs->result[0]) = ret;
	return 0;
}

