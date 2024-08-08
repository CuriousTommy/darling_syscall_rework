# Darling Syscall Rework Project

The goal of this repo is to explore the idea of reimplementing Darling's syscall implementation in a similiar fashion how XNU handles syscalls. See [motivation.md](doc/motivation.md) for more details.

## Build Instructions

```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
```