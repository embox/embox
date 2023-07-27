#!/bin/sh

QEMU=./scripts/qemu/auto_qemu
IMG=$(mktemp -d)/hda_vfat.img

dd if=/dev/zero of="$IMG" bs=1M count=64
/sbin/mkfs.vfat -n "LINUX" "$IMG" -I

# Using subshell to avoid manual cd back
(
    cd "$TEST_EMBOX_ROOT" || { echo "Failed to cd to $TEST_EMBOX_ROOT"; exit 1; }
    $QEMU -hda "$IMG"
    rm -r "$(dirname "$IMG")"
) || exit 1
