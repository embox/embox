#!/bin/bash
sudo python /opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin/esptool.py --port /dev/ttyUSB0 write_flash 0x00000 "build/base/bin/embox-0x00000.bin"
