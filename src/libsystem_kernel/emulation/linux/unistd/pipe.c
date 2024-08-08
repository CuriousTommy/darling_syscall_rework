#include "pipe.h"
#include "../base.h"
#include "../errno.h"

#if defined(__linux__)
#include <linux-syscalls/linux.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <errno.h>
#endif

int sys_pipe(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs)
{
	retargs->result_type = DARLING_SYSCALL_RETARG_TYPE_INT_T;

	int fd[2];
	int err;

#if defined(__linux__)
	#if defined(__NR_pipe)
		ret = LINUX_SYSCALL(__NR_pipe, fd);
	#else
		ret = LINUX_SYSCALL(__NR_pipe2, fd, 0);
	#endif
	if (ret < 0)
		return errno_linux_to_bsd(ret);

#if defined(__i386__) || defined(__x86_64__)
	__asm__ __volatile__("movl %0, %%edx" :: "m"(fd[1]) : "edx");
#else
#	warning Missing assembly!
#endif
	return fd[0];
#elif defined(__APPLE__)
	err = pipe(fd);
	if (err < 0) {
		return errno;
	}
#endif

	retargs->result[0] = fd[0];
	retargs->result[1] = fd[1];
	return 0;
}

