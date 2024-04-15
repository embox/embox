#!/bin/bash

echo 'Создание пустого раздела ext4 1Gb'
dd if=/dev/zero of=rootfs.ext4 bs=1M count=512
mkfs.ext4 rootfs.ext4
echo 'Создание образа sdcard.img'
mkimage -C none -A arm -T script -d boot.cmd boot.scr
./genimage --rootpath rootfs.ext4 --inputpath ./ --outputpath ./


