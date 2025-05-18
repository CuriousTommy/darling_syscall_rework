#include <darling/emulation/xnu_syscall/bsd/impl/unistd/write.h>

#include <darling/emulation/common/base.h>
#include <darling/emulation/conversion/errno.h>
#include <darling/emulation/linux_premigration/linux-syscalls/linux.h>
#include <darling/emulation/xnu_syscall/bsd/helper/bsdthread/cancelable.h>

int sys_write(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs)
{
	CANCELATION_POINT();
	return sys_write_nocancel(args, retargs);
}

// __attribute__((visibility("default")))
// long __write_for_xtrace(int fd, const void* mem, int len)
// {
// 	return sys_write_nocancel(fd, mem, len);
// }


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
