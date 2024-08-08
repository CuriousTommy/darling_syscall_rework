#include "syscall.h"

#include <errno.h>

int sys_syscall(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs)
{
	// If you look at the syscalls.master file, you'll notice that that the 
	// unix syscall [0] actually calls the nosys method. This implies that 
	// nested indirect syscalls are not allowed.

	// In XNU, the code for handling indirect syscalls usually lives in the
	// unix syscall function.

	// Should we send a SIGSYS signal?
	return ENOSYS;
}

