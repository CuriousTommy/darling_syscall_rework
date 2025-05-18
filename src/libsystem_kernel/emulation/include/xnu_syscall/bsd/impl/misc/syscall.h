#ifndef LINUX_SYSCALL_H
#define LINUX_SYSCALL_H

#include <darling/emulation/xnu_syscall/syscall_function.h>

int sys_syscall(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs);

#endif // LINUX_SYSCALL_H
