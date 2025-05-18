project(emulation)

set(common_sources
    src/common/base.c
)

set(conversion_sources
    src/conversion/network/duct.c
	src/conversion/errno.c
)

set(linux_premigration_sources
    src/linux_premigration/linux-syscall.S
    src/linux_premigration/vchroot_userspace.c
)

set(xnu_syscall_bsd_sources
    src/xnu_syscall/bsd/helper/network/duct.c
    src/xnu_syscall/bsd/impl/misc/syscall.c
    src/xnu_syscall/bsd/impl/network/recvfrom.c
    src/xnu_syscall/bsd/impl/unistd/pipe.c
    src/xnu_syscall/bsd/impl/unistd/write.c
)

set(xnu_syscall_sources
    src/xnu_syscall/syscall_asmbridge.S
	src/xnu_syscall/syscall_cbridge.c
    src/xnu_syscall/syscall_table.c
    src/xnu_syscall/syscall.c
	${xnu_syscall_bsd_sources}
)

set(emulation_sources
    ${common_sources}
    ${conversion_sources}
    ${linux_premigration_sources}
    ${xnu_syscall_sources}
)

add_library(emulation ${emulation_sources})
