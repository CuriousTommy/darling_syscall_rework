#
# Manual test cases
#

add_executable(print_temp_regs print_temp_regs/main.s)
target_link_libraries(print_temp_regs emulation)
set_target_properties(print_temp_regs PROPERTIES LINKER_LANGUAGE C)

#
# Automated test cases
#

# For executation on macOS only
if (APPLE)
add_executable(sys_write_macos_successful
    sys_write/successful_run.c
    sys_write/darling_write_macos.s
    shared/errno_stub_macos.c
)
add_executable(sys_write_macos_fail
    sys_write/fail_run.c
    sys_write/darling_write_macos.s
    shared/errno_stub_macos.c
)
add_executable(sys_syscall_sysnum_with_class_identifier_macos_fail
    indirect_syscall/fail_sysnum_with_class_identifier.c
    shared/indirect_syscall_macos.s
    shared/errno_stub_macos.c
)
add_executable(sys_syscall_recvfrom_and_sendto_macos_successful
    indirect_syscall/successful_recvfrom_and_sendto.c
    shared/indirect_syscall_macos.s
    shared/errno_stub_macos.c
)
add_executable(generic_syscall_out_of_bounds_macos_fail
    generic_syscall/fail_out_of_bound_unix_syscall.c
    generic_syscall/asm_out_of_bounds_unix_macos.s
    shared/errno_stub_macos.c
)
add_executable(sys_pipe_macos_successful
    sys_pipe/sucessful_run.c
    sys_pipe/darling_pipe_macos.s
    shared/errno_stub_macos.c
)

add_test(NAME sys_write_macos_successful COMMAND sys_write_macos_successful)
add_test(NAME sys_write_macos_fail COMMAND sys_write_macos_fail)
add_test(NAME sys_syscall_sysnum_with_class_identifier_macos_fail COMMAND sys_syscall_sysnum_with_class_identifier_macos_fail)
add_test(NAME sys_syscall_recvfrom_and_sendto_macos_successful COMMAND sys_syscall_recvfrom_and_sendto_macos_successful)
add_test(NAME generic_syscall_out_of_bounds_macos_fail COMMAND generic_syscall_out_of_bounds_macos_fail)
add_test(NAME sys_pipe_macos_successful COMMAND sys_pipe_macos_successful)
endif(APPLE)

# For executation on macOS & Linux
add_executable(sys_write_darling_successful sys_write/successful_run.c)
add_executable(sys_write_darling_fail sys_write/fail_run.c)
add_executable(sys_syscall_sysnum_with_class_identifier_darling_fail indirect_syscall/fail_sysnum_with_class_identifier.c)
add_executable(sys_syscall_recvfrom_and_sendto_darling_successful indirect_syscall/successful_recvfrom_and_sendto.c)
add_executable(generic_syscall_out_of_bounds_darling_fail 
    generic_syscall/fail_out_of_bound_unix_syscall.c
    generic_syscall/asm_out_of_bounds_unix_darling.s
)
add_executable(sys_pipe_darling_successful sys_pipe/sucessful_run.c)

target_link_libraries(sys_write_darling_successful system_kernel)
target_link_libraries(sys_write_darling_fail system_kernel)
target_link_libraries(sys_syscall_sysnum_with_class_identifier_darling_fail system_kernel)
target_link_libraries(sys_syscall_recvfrom_and_sendto_darling_successful system_kernel)
target_link_libraries(generic_syscall_out_of_bounds_darling_fail system_kernel)
target_link_libraries(sys_pipe_darling_successful system_kernel)

add_test(NAME sys_write_darling_successful COMMAND sys_write_darling_successful)
add_test(NAME sys_write_darling_fail COMMAND sys_write_darling_fail)
add_test(NAME sys_syscall_sysnum_with_class_identifier_darling_fail COMMAND sys_syscall_sysnum_with_class_identifier_darling_fail)
add_test(NAME sys_syscall_recvfrom_and_sendto_darling_successful COMMAND sys_syscall_recvfrom_and_sendto_darling_successful)
add_test(NAME generic_syscall_out_of_bounds_darling_fail COMMAND generic_syscall_out_of_bounds_darling_fail)
add_test(NAME sys_pipe_darling_successful COMMAND sys_pipe_darling_successful)