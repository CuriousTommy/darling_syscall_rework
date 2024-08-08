#ifndef _ASM_GENERAL_UNISTD_H
#define _ASM_GENERAL_UNISTD_H

#include "../base.h"

VISIBLE
int __linux_syscall(int nr, ...);

#define LINUX_SYSCALL0(nr) linux_syscall(0, 0, 0, 0, 0, 0, nr)
#define LINUX_SYSCALL1(nr, a1) linux_syscall((long)a1, 0, 0, 0, 0, 0, nr)
#define LINUX_SYSCALL2(nr, a1, a2) linux_syscall((long)a1, (long)a2, 0, 0, 0, 0, nr)
#define LINUX_SYSCALL3(nr, a1, a2, a3) linux_syscall((long)a1, (long)a2, (long)a3, 0, 0, 0, nr)
#define LINUX_SYSCALL4(nr, a1, a2, a3, a4) linux_syscall((long)a1, (long)a2, (long)a3, (long)a4, 0, 0, nr)
#define LINUX_SYSCALL5(nr, a1, a2, a3, a4, a5) linux_syscall((long)a1, (long)a2, (long)a3, (long)a4, (long)a5, 0, nr)
#define LINUX_SYSCALL6(nr, a1, a2, a3, a4, a5, a6) linux_syscall((long)a1, (long)a2, (long)a3, (long)a4, (long)a5, (long)a6, nr)

// Syscall numbers
# ifdef __i386__
//  # include <asm/unistd_32.h>
    # include "sysnum/linux-x86.h"
# elif defined(__x86_64__)
//  # include <asm/unistd_64.h>
    # include "sysnum/linux-x86_64.h"
# elif defined(__arm64__)
//  # include <asm/unistd.h>
    # include "sysnum/linux-arm64.h"
# else
//  look in <asm/unistd.h> if you are unsure
    # error Missing Linux sc numbers for this arch
# endif

#endif /* _ASM_GENERAL_UNISTD_H */
