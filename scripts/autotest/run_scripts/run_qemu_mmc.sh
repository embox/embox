#!/bin/bash

QEMU=./scripts/qemu/auto_qemu
IMG=$(mktemp -d)/mmc.img

./scripts/disk/partitions.sh "$IMG"

# Using subshell to avoid having to cd back
(
    cd "$TEST_EMBOX_ROOT" || { echo "Failed to cd to $TEST_EMBOX_ROOT"; exit 1; }
    $QEMU -sd "$IMG" "$@"
    rm -r "$(dirname "$IMG")"
) || exit 1
