#ifndef LINUX_PIPE_H
#define LINUX_PIPE_H

#include "../syscall_function.h"

int sys_pipe(const darling_syscall_args_t* args, darling_syscall_retarg_t* retargs);

#endif
