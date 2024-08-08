project(darling-libsystem_kernel)

add_subdirectory(emulation/linux)
add_subdirectory(libsyscall)

add_library(system_kernel stub.c)
target_link_libraries(system_kernel emulation libsyscall)
