project(libsyscall)

add_library(libsyscall
    bsdsyscalls/___pipe.S
    bsdsyscalls/___recvmsg.S
    bsdsyscalls/___syscall.S
    bsdsyscalls/_write.S

    custom/errno.c
)

# Needed for Linux
target_link_libraries(libsyscall emulation)