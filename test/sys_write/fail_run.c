#include <sys/types.h>
#include <assert.h>
#include <string.h>

extern ssize_t darling_write(int fd, const void* buf, size_t nbyte);

extern void darling_set_cerror(int err);
extern int darling_get_cerror();

int main() {
    darling_set_cerror(0);

    const char* hello_world_str = "Hello World!"; 
    size_t hello_world_expected_len =  strlen(hello_world_str);
    size_t actual_len = darling_write(1000, hello_world_str, hello_world_expected_len);

    int expected_cerror = 9; // Bad File Descriptor
    assert(darling_get_cerror() == expected_cerror);
}
