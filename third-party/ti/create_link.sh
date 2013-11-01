#!/bin/sh

SYSLINK_EMBOX=$(realpath $1)

if [ ! -d $SYSLINK_EMBOX ]; then
	echo "setup path to syslink_embox directory"
       	exit 1
fi

cd $(dirname $0)

ln -s $SYSLINK_EMBOX/ipc_1_24_00_16 ./
ln -s $SYSLINK_EMBOX/syslink_2_21_01_05 ./
