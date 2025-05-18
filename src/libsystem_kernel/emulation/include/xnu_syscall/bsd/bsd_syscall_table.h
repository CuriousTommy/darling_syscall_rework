#ifndef LINUX_SYSCALLS_H
#define LINUX_SYSCALLS_H

#include <darling/emulation/xnu_syscall/syscall_table.h>

extern darling_syscall_metadata_t __unix_syscall_table[DARLING_SYSCALLTABLE_UNIX_MAXSIZE];

#endif // LINUX_SYSCALLS_H
