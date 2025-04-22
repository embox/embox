#!/bin/sh

FS_TYPE=ext3

QEMU=./scripts/qemu/auto_qemu
IMG=$(mktemp -d)/hda_$FS_TYPE.img

dd if=/dev/zero of="$IMG" bs=1M count=64
/sbin/mkfs.$FS_TYPE -F -b 1024 "$IMG"

# Using subshell to avoid manual cd back
(
    cd "$TEST_EMBOX_ROOT" || { echo "Failed to cd to $TEST_EMBOX_ROOT"; exit 1; }
    $QEMU -hda "$IMG"
    rm -r "$(dirname "$IMG")"
) || exit 1
