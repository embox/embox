#!/bin/sh

IMAGE=build/base/bin/embox

arm-none-eabi-gdb -x $(dirname $0)/da14695_gdb_start_script $IMAGE
