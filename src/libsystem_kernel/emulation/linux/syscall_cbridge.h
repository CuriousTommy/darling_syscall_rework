#ifndef DARLING_CSYSCALL_H
#define DARLING_CSYSCALL_H

#include "syscall.h"

#include <stdint.h>

#if defined(__x86_64__)
typedef struct darling_register_x86_64 {
	// Temporary Registers
    uint64_t rax;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;

    // x86 flags
    uint64_t rflag;

	// Stack/Frame Pointer
    uint64_t rbp;
	uint64_t rsp;
} darling_register_x86_64_t;

#else
#error "Missing register struct for arch"
#endif

#endif // DARLING_CSYSCALL_H