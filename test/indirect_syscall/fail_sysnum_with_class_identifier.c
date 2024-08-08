#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

extern void darling_asm_out_of_bounds();

extern void darling_set_cerror(int err);
extern int darling_get_cerror();

void sigsys_handler(int value);

extern int darling_syscall(int, ...);

int main() {
    darling_set_cerror(0);

    struct sigaction sigaction_struct = {0};
    sigaction_struct.__sigaction_u.__sa_handler = sigsys_handler;
    sigaction(SIGSYS, &sigaction_struct, NULL);

    int fd = 1;
    const char* hello_world_str = "Hello World!\n"; 
    size_t hello_world_expected_len =  strlen(hello_world_str);
    int result = darling_syscall(0x2000004, fd, hello_world_str, hello_world_expected_len);

    assert(darling_get_cerror() == ENOSYS);
}

void sigsys_handler(int value) {
    // Apple's offical syscall implementation throws a SIGSYS, but we won't
    // worry about that for our darling implementation. In other words, we
    // won't verify that SIGSYS is thrown and will just ignore it.
    printf("In sigsys_handler(%d)\n", value);
}