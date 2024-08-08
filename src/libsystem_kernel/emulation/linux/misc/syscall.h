#ifndef LINUX_SYSCALL_H
#define LINUX_SYSCALL_H

#include "../syscall_function.h"

int sys_syscall(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs);

#endif

