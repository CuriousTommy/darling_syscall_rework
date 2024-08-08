#ifndef LINUX_WRITE_H
#define LINUX_WRITE_H

#include "../syscall_function.h"

int sys_write(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs);
int sys_write_nocancel(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs);

#endif // LINUX_WRITE_H
