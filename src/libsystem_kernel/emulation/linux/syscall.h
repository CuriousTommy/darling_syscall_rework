#ifndef SYSCALL_H
#define SYSCALL_H

#include "syscall_function.h"

#include <stdbool.h>

typedef struct darling_syscall_args_builder {
    // Data
    darling_syscall_args_t args;
    bool indirect_syscall;
    void* arch_specific_regs;
    int arg_count;

    // Function
    void (*construct_full_arguments)(struct darling_syscall_args_builder* builder);
} darling_syscall_args_builder_t;

int darling_common_syscall_unix(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs);
int darling_common_syscall_mach(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs);
int darling_common_syscall_machdep(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs);
int darling_common_syscall_diag(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs);

int darling_sysnum_unimplemented(const char* syscall_type, int sysnum);
int darling_sysnum_out_of_range(const char* syscall_type, int sysnum);

#endif // SYSCALL_H