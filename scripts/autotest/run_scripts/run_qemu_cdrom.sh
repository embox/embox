#!/bin/sh

QEMU=./scripts/qemu/auto_qemu
IMG=$(mktemp -d)/isofs.img

sudo mkisofs -o "$IMG" conf/

# Using subshell to avoid manual cd back
(
    cd "$TEST_EMBOX_ROOT" || { echo "Failed to cd to $TEST_EMBOX_ROOT"; exit 1; }
    $QEMU -cdrom "$IMG"
    rm -r "$(dirname "$IMG")"
) || exit 1
