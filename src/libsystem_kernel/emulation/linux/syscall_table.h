#ifndef SYSCALL_TABLE_H
#define SYSCALL_TABLE_H

#include "syscall_function.h"

#define DARLING_SYSCALLTABLE_UNIX_MAXSIZE 600
#define DARLING_SYSCALLTABLE_MACH_MAXSIZE 128
#define DARLING_SYSCALLTABLE_MACHDEP_MAXSIZE 10

typedef struct darling_syscall_metadata {
    darling_syscall_func_t func;
    int arg_count;
} darling_syscall_metadata_t;

extern darling_syscall_metadata_t __unix_syscall_table[DARLING_SYSCALLTABLE_UNIX_MAXSIZE];
extern darling_syscall_metadata_t __mach_syscall_table[DARLING_SYSCALLTABLE_MACH_MAXSIZE];
extern darling_syscall_metadata_t __machdep_syscall_table[DARLING_SYSCALLTABLE_MACHDEP_MAXSIZE];

#endif // SYSCALL_TABLE_H