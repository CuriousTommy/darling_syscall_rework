#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

extern void darling_asm_out_of_bounds();

extern void darling_set_cerror(int err);
extern int darling_get_cerror();

void sigsys_handler(int value);

int main() {
    darling_set_cerror(0);

#if defined(__APPLE__)
    struct sigaction sigaction_struct = {0};
    sigaction_struct.__sigaction_u.__sa_handler = sigsys_handler;
    sigaction(SIGSYS, &sigaction_struct, NULL);
#else
    struct sigaction sigaction_struct = {0};
    sigaction_struct.sa_handler = sigsys_handler;
    sigaction(SIGSYS, &sigaction_struct, NULL);
#endif

    darling_asm_out_of_bounds();

    assert(darling_get_cerror() == ENOSYS);
}

void sigsys_handler(int value) {
    // Apple's offical syscall implementation throws a SIGSYS, but we won't
    // worry about that for our darling implementation. In other words, we
    // won't verify that SIGSYS is thrown and will just ignore it.
    printf("In sigsys_handler(%d)\n", value);
}