#ifndef LINUX_RECVFROM_H
#define LINUX_RECVFROM_H

#include "../syscall_function.h"

int sys_recvfrom(const struct darling_syscall_args* args, struct darling_syscall_retarg* retargs);
int sys_recvfrom_nocancel(const struct darling_syscall_args* args, struct darling_syscall_retarg* retargs);

#define LINUX_SYS_RECVFROM	12

#endif

