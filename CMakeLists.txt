cmake_minimum_required(VERSION 3.28.0)
project(darling_syscall_rework
    LANGUAGES
        C ASM
)

include(CTest)
set(CTEST_OUTPUT_ON_FAILURE TRUE)

include_directories(include)

add_definitions("-DPRIVATE -DDARLING")

add_subdirectory(src/libsystem_kernel)
add_subdirectory(test)