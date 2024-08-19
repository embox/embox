#!/bin/sh

#OPENOCD=~/git/openocd/src/openocd
#MIK32_UPLOADER=~/mikron/mik32-uploader

OPENOCD=$1
MIK32_UPLOADER=$2

echo openocd="$OPENOCD"
echo mik32_uploader="$MIK32_UPLOADER"

sudo $OPENOCD -f $MIK32_UPLOADER/openocd-scripts/interface/ftdi/mikron-link.cfg -f $MIK32_UPLOADER/openocd-scripts/target/mik32.cfg -c "init;halt"
