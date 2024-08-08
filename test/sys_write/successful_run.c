#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

extern ssize_t darling_write(int fd, const void* buf, size_t nbyte);

extern void darling_set_cerror(int err);
extern int darling_get_cerror();

int main() {
    darling_set_cerror(0);

    int pipefd[2];
    pipe(pipefd);

    const char* hello_world_str = "Hello World!"; 
    size_t hello_world_expected_len =  strlen(hello_world_str);
    size_t actual_len = darling_write(pipefd[1], hello_world_str, hello_world_expected_len);
    assert(hello_world_expected_len == actual_len);

    char expected_result[100] = "ABCDEFJH";
    ssize_t actual_len2 = read(pipefd[0], expected_result, sizeof(expected_result));
    assert(hello_world_expected_len == actual_len2);
    assert(strcmp(hello_world_str,expected_result) == 0);
    
    assert(darling_get_cerror() == 0);
    return 0;
}