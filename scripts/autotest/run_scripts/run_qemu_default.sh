#!/bin/sh

QEMU=./scripts/qemu/auto_qemu

# Using subshell to avoid having to cd back
(
    cd "$TEST_EMBOX_ROOT" || { echo "Failed to cd to $TEST_EMBOX_ROOT"; exit 1; }
    $QEMU
) || exit 1
