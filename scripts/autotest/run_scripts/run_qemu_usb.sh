#!/usr/bin/env bash

QEMU=./scripts/qemu/auto_qemu
image_file_name=usbdisk.img

qemu-img create -f qcow2 $image_file_name 512M

/sbin/mkfs.vfat $image_file_name

cd $TEST_EMBOX_ROOT
$QEMU -drive id=my_usb_disk,file=$image_file_name,if=none,format=raw -device usb-storage,drive=my_usb_disk $@
cd -
