#ifndef SYSCALL_FUNCTION_H
#define SYSCALL_FUNCTION_H

#include <stdint.h>

// Based on [xnu]/bsd/arm/types.h & [xnu]/bsd/i386/types.h
#if defined(__x86_64__)
typedef void*    darling_syscall_addr_t;
typedef uint64_t darling_syscall_size_t;
typedef int64_t  darling_syscall_ssize_t;
typedef int64_t  darling_syscall_long_t;
typedef uint64_t darling_syscall_ulong_t;
typedef int64_t  darling_syscall_time_t;
typedef int64_t  darling_syscall_off_t;
#else
#error "Missing type definitions for arch
#endif

typedef enum darling_syscall_retarg_type {
	DARLING_SYSCALL_RETARG_TYPE_INVALID,
	DARLING_SYSCALL_RETARG_TYPE_NONE,
	DARLING_SYSCALL_RETARG_TYPE_INT_T,
	DARLING_SYSCALL_RETARG_TYPE_UINT_T,
	DARLING_SYSCALL_RETARG_TYPE_OFF_T,
	DARLING_SYSCALL_RETARG_TYPE_ADDR_T,
	DARLING_SYSCALL_RETARG_TYPE_SIZE_T,
	DARLING_SYSCALL_RETARG_TYPE_SSIZE_T,
	DARLING_SYSCALL_RETARG_TYPE_UINT64_T
} darling_syscall_retarg_type_t;

typedef enum darling_syscall_type {
	DARLING_SYSCALL_TYPE_INVALID,
	DARLING_SYSCALL_TYPE_UNIX,
	DARLING_SYSCALL_TYPE_MACH,
	DARLING_SYSCALL_TYPE_MACHDEP
} darling_syscall_type_t;

// The way Apple handles syscall arguments (inside the kernel) can vary.
// If we take a look at x86_64 for example, Mach and Unix arguments are 
// stored inside a struct/array, while Machdep arguments are handled by 
// having multiple function pointers for the amount of arguments needed.
// 
// To keep things simple, we will have all of the syscalls (Mach, Unix,
// Machdep, etc.) use the `darling_syscall_args` struct to handle the 
// arguments.
//
// `darling_syscall_args` is based on the following:
// * Universal
//   * u_int64_t uu_arg[8] | https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/sys/user.h#L118
// * i386/x86_64
//   * struct mach_call_args | https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/bsd_i386.c#L452-L462
//   * typedef union {...} machdep_call_routine_t | https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/osfmk/i386/machdep_call.h#L39-L53

typedef struct darling_syscall_args {
    int sysnum;
	uint64_t arg[9];
} darling_syscall_args_t;

// Just like with `darling_syscall_args`, the way return values are handled, can
// vary between syscall types. On x86_64, they can be stored in a struct/passed by
// reference (Unix/Machdep), or returned from the function (Mach/Machdep).
//
// In addition, the following members have been added: `ret_type` & `syscall_type`
// * `ret_type` is used to determine how the values should be stored back into the 
//    registers.
// ` is added to determine how the returned values
// should be handled. This is a subsititue to how the XNU kernel relies on 
// `callp->sy_return_type` to figure out this information.
//
// `darling_syscall_retarg` is based on:
// * int uu_rval[2] | https://github.com/apple-oss-distributions/xnu/blob/xnu-7195.141.2/bsd/sys/user.h#L119

typedef struct darling_syscall_retarg {
	darling_syscall_type_t syscall_type;
	darling_syscall_retarg_type_t result_type;
	int result[2];
} darling_syscall_retarg_t;

typedef int (*darling_syscall_func_t)(const darling_syscall_args_t*,darling_syscall_retarg_t*);

#endif // SYSCALL_FUNCTION_H