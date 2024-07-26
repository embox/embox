#!/bin/sh

OPENOCD=~/git/openocd/src/openocd
MIK32_UPLOADER=~/mikron/mik32-uploader

sudo $OPENOCD -f $MIK32_UPLOADER/openocd-scripts/interface/ftdi/mikron-link.cfg -f $MIK32_UPLOADER/openocd-scripts/target/mik32.cfg -c "init;halt"
