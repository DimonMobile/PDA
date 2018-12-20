#!/bin/bash
set -e

as -gstabs ./build/Debug/result.s -o ./build/Debug/result.o
ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 -lc ./build/Debug/result.o -o ./build/Debug/res
