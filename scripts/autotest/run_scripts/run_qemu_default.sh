#!/bin/sh

QEMU=./scripts/qemu/auto_qemu

cd $TEST_EMBOX_ROOT
$QEMU
cd -
