#!/bin/bash

echo 'Create empty partition ext4 1Gb'
mkfs.ext4 rootfs.ext4
dd if=/dev/zero of=rootfs.ext4 bs=1M count=1024

echo 'Create sdcard.img'
mkimage -C none -A arm -T script -d boot.cmd boot.scr
genimage --rootpath rootfs.ext4 --inputpath ./ --outputpath ./


