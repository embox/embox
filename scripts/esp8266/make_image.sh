#!/bin/bash
esptool.py elf2image --flash_size 4m --flash_mode dio -e 1 -o "build/base/bin/embox-" "build/base/bin/embox"
