#!/bin/sh

[ -f $1 ] || (echo "setup path to syslink_embox directory"; exit 1;)

ln -s $1/ipc_1_24_00_16 ipc_1_24_00_16
ln -s $1/syslink_2_21_01_05

