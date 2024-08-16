#include "syscall_cbridge.h"

#include "simple.h"
#include "syscall.h"

#include <mach/machine/syscall_sw.h>

#include <sys/errno.h>
#include <stdint.h>
#include <stdio.h>

// HACK/WORKAROUND
#define	ERESTART	(-1)
#define	EJUSTRETURN	(-2)

#if defined(__x86_64__)
#define X86_64_FLAG_CARRY 0x1

#define PUSH_RSP_SIZE 0x8
#define PUSH_RETURN_ADDRESS_SIZE 0x8

void darling_x86_64_syscall_full_args_init(darling_syscall_args_builder_t *builder);
void darling_x86_64_syscall_partial_args_init(darling_syscall_args_builder_t *builder);
void darling_x86_64_syscall_set_return_values(const darling_syscall_retarg_t* retarg, darling_register_x86_64_t* regs, int error);

int get_sysnum(uint64_t reg) { return SYSCALL_NUMBER_MASK & (int)reg; }

int _darling_handle_x86_64_syscall(darling_register_x86_64_t* regs) {
    // TODO: REMOVE
    printf("In _darling_handle_x86_64_syscall\n");
    printf("regs->rax = %llu\n", regs->rax);
    printf("regs->rdi = %llu\n", regs->rdi);
    printf("regs->rsi = %llu\n", regs->rsi);
    printf("regs->rdx = %llu\n", regs->rdx);
    printf("regs->rcx = %llu\n", regs->rcx);
    printf("regs->r8 = %llu\n", regs->r8);
    printf("regs->r9 = %llu\n", regs->r9);
    printf("regs->r10 = %llu\n", regs->r10);
    printf("regs->r11 = %llu\n", regs->r11);
    printf("regs->rflag = %llu\n", regs->rflag);
    printf("regs->rsp = %llu\n", regs->rsp);
    printf("regs->rbp = %llu\n", regs->rbp);

    regs->rflag &= ~X86_64_FLAG_CARRY;

    darling_syscall_args_builder_t builder;
    darling_syscall_retarg_t retargs;
    int error = 0;

    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/dev/i386/systemcalls.c#L331
    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L637
    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L389
    
    builder.arch_specific_regs = regs;
    builder.construct_full_arguments = darling_x86_64_syscall_full_args_init;
    darling_x86_64_syscall_partial_args_init(&builder);

    retargs.syscall_type = DARLING_SYSCALL_TYPE_INVALID;
    retargs.result_type = DARLING_SYSCALL_RETARG_TYPE_INVALID;

    int class_mask = (SYSCALL_CLASS_MASK & (int)regs->rax) >> SYSCALL_CLASS_SHIFT;
    switch (class_mask) {
    case SYSCALL_CLASS_MACH:
        error = darling_common_syscall_mach(&builder,&retargs);
        break;
    case SYSCALL_CLASS_UNIX:
        error = darling_common_syscall_unix(&builder,&retargs);
        break;
    case SYSCALL_CLASS_MDEP:
        error = darling_common_syscall_machdep(&builder,&retargs);
        break;
    case SYSCALL_CLASS_DIAG:
        error = darling_common_syscall_diag(&builder,&retargs);
        break;
    default:
        // macOS doesn't seem to throw any type of error in userspace when it comes
        // to providing an unknown syscall class type.
        __simple_printf("Unknown syscall class type (%d)\n", class_mask);
    }

    darling_x86_64_syscall_set_return_values(&retargs,regs,error);

    return error;
}

// This function will only copy over the arguments that live in the regsiter.
// We will not worry about handling indirect syscall in this function.
void darling_x86_64_syscall_partial_args_init(darling_syscall_args_builder_t *builder) {
    darling_syscall_args_t *args = &builder->args;
    darling_register_x86_64_t* regs = (darling_register_x86_64_t*)builder->arch_specific_regs;


    args->sysnum = get_sysnum(regs->rax);

    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/dev/i386/systemcalls.c#L359
    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L659-L660
    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L413-L419
    args->arg[0] = regs->rdi;
    args->arg[1] = regs->rsi;
    args->arg[2] = regs->rdx;
    args->arg[3] = regs->r10;
    args->arg[4] = regs->r8;
    args->arg[5] = regs->r9;
}

void darling_x86_64_syscall_full_args_init(darling_syscall_args_builder_t *builder) {
    darling_syscall_args_t *args = &builder->args;
    darling_register_x86_64_t* regs = (darling_register_x86_64_t*)builder->arch_specific_regs;
    int param_size = builder->arg_count;

    // Only Unix syscalls support indirect syscalls
    bool indirect_syscall = builder->indirect_syscall;

    if (indirect_syscall) {
        // Looking at the XNU code, my understanding is that the first argument
        // should just be the sysnum. We don't need to worry about the syscall class.
        args->sysnum = regs->rdi;

        args->arg[0] = regs->rsi;
        args->arg[1] = regs->rdx;
        args->arg[2] = regs->r10;
        args->arg[3] = regs->r8;
        args->arg[4] = regs->r9;
        
        // If we are dealing with an indirect syscall, we need to account for the 6th
        // argument being shifted into the stack. This shouldn't cause issues with methods,
        // that don't take any arguments, since the arg_count is below 6.
        param_size++;
    }

    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/dev/i386/systemcalls.c#L373
    // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L668
    if (param_size > 6) {
        int max_params = sizeof(args->arg)/sizeof(args->arg[0]) + (indirect_syscall ? 1 : 0);
        if (param_size > max_params) {
            __builtin_trap();
        }

        // Since we are faking the behavior of a real syscall, we have to adjust for the actual 
        // location of our additional arguments. 
        // 
        // See `syscall_asmbridge.S` and Darling's definition of `UNIX_SYSCALL_SYSCALL`
        // to better understand the logic behind `(PUSH_RETURN_ADDRESS_SIZE + PUSH_RSP_SIZE) * 2`
        uint64_t* rsp_ptr = (uint64_t*)((char*)regs->rsp + (PUSH_RETURN_ADDRESS_SIZE + PUSH_RSP_SIZE) * 2);

        // To understand what is going on here, we need to understand how x86_64 handles additional
        // arguments that can't be stored in a register. For example, let's look that this function:
        // ```c
        // extern void call_me(char a1, char b2, char c3, char d4, char e5, char f6, char g7,
        //                     short h8, int i9, long j10);
        // call_me(1,2,3,4,5,6,7,8,9,10);
        // ```
        // 
        // If we look at the assembly generated for `call_me()`...
        // ```asm
        // movl    $1, %edi
        // movl    $2, %esi
        // movl    $3, %edx
        // movl    $4, %ecx
        // movl    $5, %r8d
        // movl    $6, %r9d
        // pushq   $10
        // pushq   $9
        // pushq   $8
        // pushq   $7
        // callq   call_me@PLT
        // ```
        // 
        // We would notice the following:
        // * The 7th to 10th argument are pushed to the stack
        // * The compiler always uses `pushq`, reguardless of the argument's size
        // * The order of the arguments pushed to the stack goes from 10th first to
        //   7th last

        int starting_param = 6;
        int starting_arg = indirect_syscall ? 5 : 6;
        for (int i=0; starting_param+i < param_size; i++, rsp_ptr++) {
            args->arg[starting_arg+i] = *rsp_ptr;
        }
    }
}

void darling_x86_64_syscall_set_return_values(const darling_syscall_retarg_t* retarg, darling_register_x86_64_t* regs, int error) {
    if (retarg->syscall_type == DARLING_SYSCALL_TYPE_UNIX) {
        // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/dev/i386/systemcalls.c#L429-L439
        if (error != 0 && error != ERESTART && error != EJUSTRETURN) {
            regs->rax = error;
            regs->rflag |= X86_64_FLAG_CARRY;
            return;
        }

        // https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/dev/i386/systemcalls.c#L441-L464
        switch (retarg->result_type) {
        case DARLING_SYSCALL_RETARG_TYPE_INT_T:
            regs->rax = retarg->result[0];
            regs->rdx = retarg->result[1];
            return;
        case DARLING_SYSCALL_RETARG_TYPE_UINT_T:
            regs->rax = ((uint32_t)retarg->result[0]);
            regs->rdx = ((uint32_t)retarg->result[1]);
            return;
        case DARLING_SYSCALL_RETARG_TYPE_OFF_T:
        case DARLING_SYSCALL_RETARG_TYPE_ADDR_T:
        case DARLING_SYSCALL_RETARG_TYPE_SIZE_T:
        case DARLING_SYSCALL_RETARG_TYPE_SSIZE_T:
        case DARLING_SYSCALL_RETARG_TYPE_UINT64_T:
            regs->rax = *((uint64_t*)(&retarg->result[0]));
            regs->rdx = 0;
            return;
        case DARLING_SYSCALL_RETARG_TYPE_NONE:
            return;
        default:
            __builtin_trap();
        }
    }
}

#else
#error "Missing C Syscall Bridge"
#endif