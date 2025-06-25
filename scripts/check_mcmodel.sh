#!/bin/bash
# scripts/check_mcmodel.sh
# Test script to detect -mcmodel=medlow relocation issues

# Use the compiler from the environment or default to riscv64-unknown-elf-gcc
CC=${CC:-riscv64-unknown-elf-gcc}

# Test compilation and linking with -mcmodel=medlow
echo "Checking -mcmodel=medlow compatibility..."
if ! $CC -mcmodel=medlow -o mcmodel_test scripts/mcmodel_test.c 2>/dev/null; then
    echo "Warning: Linking with -mcmodel=medlow failed. This indicates your toolchain or libraries may not support large code/data sizes (>1 MiB)."
    echo "Recommendation: Rebuild your toolchain with --with-cmodel=medany or use -mcmodel=medany for your application."
    echo "To rebuild the toolchain, run: ./configure --with-cmodel=medany && make"
    exit 1
else
    echo "Success: -mcmodel=medlow is compatible with large code/data sizes."
    rm -f mcmodel_test
    exit 0
fi