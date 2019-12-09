#!/bin/bash

QEMU=./scripts/qemu/auto_qemu
IMG=$(mktemp -d)/mmc.img

./scripts/disk/partitions.sh $IMG

cd $TEST_EMBOX_ROOT
$QEMU -sd $IMG $@
rm -r $(dirname $IMG)
cd -
