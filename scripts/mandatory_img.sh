#!/bin/sh

set -evx

SZ=${1:-8M}
IMG=${2:-mandatory.img}

LABEL=SMAC32LABEL

dd if=/dev/zero bs=1 count=$SZ of=$IMG
MKE2FS_DEVICE_SECTSIZE=512 mke2fs -q -b 1024 $IMG

tmp_mount=$(mktemp -d)

mount -o loop $IMG $tmp_mount

for i in confidentially secret service unclassified; do
	echo hello > $tmp_mount/$i
	chmod 777 $tmp_mount/$i
	attr -s $LABEL -V $i $tmp_mount/$i

done

umount $tmp_mount
rmdir $tmp_mount
