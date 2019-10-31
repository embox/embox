#!/bin/sh

QEMU=./scripts/qemu/auto_qemu
IMG=$(mktemp -d)/hda_vfat.img

dd if=/dev/zero of=$IMG bs=1M count=64
/sbin/mkfs.vfat -n "LINUX" $IMG -I

cd $TEST_EMBOX_ROOT
$QEMU -hda $IMG
rm -r $(dirname $IMG)
cd -
