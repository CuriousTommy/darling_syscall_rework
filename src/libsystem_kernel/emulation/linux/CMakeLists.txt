project(emulation)

set(emulation_sources
    syscall.c
    syscall_asmbridge.S
    syscall_cbridge.c
    syscall_table.c

    errno.c
    vchroot_userspace.c

    misc/syscall.c
    network/duct.c
    network/recvfrom.c
    unistd/pipe.c
    unistd/write.c

    linux-syscalls/linux-syscall.S
)

add_library(emulation ${emulation_sources})