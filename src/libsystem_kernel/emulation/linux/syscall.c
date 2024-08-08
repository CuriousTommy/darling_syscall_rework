#include "syscall.h"

#include "simple.h"
#include "syscall_table.h"

#include <stdio.h>
#include <errno.h>

#define UNIX_SYSNUM_INDIRECT_SYSCALL 0

int darling_common_syscall_unix(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs) {
    printf("In darling_syscall_unix\n");
    retargs->syscall_type = DARLING_SYSCALL_TYPE_UNIX;

    retargs->result[0] = 0;
    retargs->result[1] = 0;

    int sysnum = builder->args.sysnum;
    darling_syscall_metadata_t* metadata = NULL;

    if (sysnum == UNIX_SYSNUM_INDIRECT_SYSCALL) {
        sysnum = builder->args.arg[0];
        builder->indirect_syscall = true;
    } else {
        builder->indirect_syscall = false;
    }

    if (sysnum < 0 || sysnum >= DARLING_SYSCALLTABLE_UNIX_MAXSIZE) {
        return darling_sysnum_out_of_range("unix",sysnum);
    }
    
    metadata = &__unix_syscall_table[sysnum];
    if (metadata->func == NULL) {
        return darling_sysnum_unimplemented("unix",sysnum);
    }

    builder->arg_count = metadata->arg_count;
    builder->construct_full_arguments(builder);
    return metadata->func(&builder->args,retargs);
}

int darling_common_syscall_mach(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs) {
    printf("In darling_syscall_mach\n");
    return 0;
}

int darling_common_syscall_machdep(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs) {
    printf("In darling_syscall_machdep\n");
    return 0;
}

int darling_common_syscall_diag(darling_syscall_args_builder_t* builder, darling_syscall_retarg_t* retargs) {
    __simple_printf("Darling does not support the diagnostics syscall, ignoring...\n");
    return 0;
}

int darling_sysnum_unimplemented(const char* syscall_type, int sysnum)
{
	__simple_printf("Unimplemented %s syscall (%d)\n", syscall_type, sysnum);
	return ENOSYS;
}

int darling_sysnum_out_of_range(const char* syscall_type, int sysnum)
{
    // Should we match nosys()'s behavior and throw a SIGSYS signal as well?
    __simple_printf("The %s syscall requested is out of range (%d)\n", syscall_type, sysnum);
	return ENOSYS;
}