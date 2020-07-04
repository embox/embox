#!/bin/sh

IMAGE=build/base/bin/embox

arm-none-eabi-gdb -x $(dirname $0)/da14680_gdb_start_script $IMAGE
