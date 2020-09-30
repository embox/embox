#!/bin/sh

arm-none-eabi-objcopy -O binary build/base/bin/embox build/base/bin/qspi.bin --only-section=.text --only-section=.rodata --only-section='.ARM.ex*' --only-section='.data'
