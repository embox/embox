#!/bin/sh
export TEST_CURRENT_CONFIG=stm32

export EMBOX_IP=192.168.1.128
export HOST_IP=192.168.1.1
export EMBOX_PROMPT=">"
export TEST_TARGET_CONNECTION_TYPE=picocom
export TEST_PICOCOM_TTY_DEV=/dev/ttyUSB0
export TEST_HOST_NFS_DIR=/var/nfs_test
