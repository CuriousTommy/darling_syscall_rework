# Darling Syscall Rework Project

The goal of this repo is to explore the idea of reimplementing Darling's syscall implementation in a similiar fashion how XNU handles syscalls. See [motivation.md](doc/motivation.md) for more details.

## Build Instructions

```
# When building of Linux, make sure to set the compiler to Clang
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```