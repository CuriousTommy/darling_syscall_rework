project(emulation)

set(emulation_sources
    syscall.c
    syscall_asmbridge.S
    syscall_cbridge.c
    syscall_table.c

    errno.c

    misc/syscall.c
    network/recvfrom.c
    unistd/pipe.c
    unistd/write.c
)

add_library(emulation ${emulation_sources})