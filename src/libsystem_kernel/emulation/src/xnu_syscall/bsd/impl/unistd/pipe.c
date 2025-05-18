#include <darling/emulation/xnu_syscall/bsd/impl/unistd/pipe.h>

#include <darling/emulation/common/base.h>
#include <darling/emulation/conversion/errno.h>
#include <darling/emulation/linux_premigration/linux-syscalls/linux.h>

int sys_pipe(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs)
{
	retargs->result_type = DARLING_SYSCALL_RETARG_TYPE_INT_T;

	int fd[2];
	int err;

#if defined(__linux__)
	#if defined(__NR_pipe)
		err = LINUX_SYSCALL(__NR_pipe, fd);
	#else
		err = LINUX_SYSCALL(__NR_pipe2, fd, 0);
	#endif

	if (err < 0)
		return errno_linux_to_bsd(err);

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
