#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

extern int darling_pipe(int fd[2]);

int main() {
    int fd[2] = { 0,0 };

    int err = darling_pipe(fd);
    assert(err == 0);

    // Upon successful completion, the value returned depends on cmd as follows:
    // F_GETFD    Value of flag (only the low-order bit is defined).
    // Otherwise, a value of -1 is returned and errno is set to indicate the error.

    int fd0_flag = fcntl(fd[0], F_GETFD);
    int fd1_flag = fcntl(fd[1], F_GETFD);
    assert(fd0_flag != -1 && fd1_flag != -1);

    char pipe_string[100] = "INVALID";
    write(fd[1], "Hello world!", 13);
    read(fd[0], pipe_string, sizeof(pipe_string));
    assert(strcmp("Hello world!", pipe_string) == 0);
}