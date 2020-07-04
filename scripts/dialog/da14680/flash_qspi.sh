#!/bin/sh

if [ -z $DIALOG_DA14680_SDK_ROOT ]; then
	printf "DIALOG_DA14680_SDK_ROOT is not set. Please specify DA14680 SDK path.\n"
	printf "  Example: export DIALOG_DA14680_SDK_ROOT=~/git/usb/DA14680/SDK\n"
	exit 1
fi

if [ -z $JLINK_ROOT ]; then
	printf "JLINK_ROOT is not set. Please specify the path.\n"
	printf "  Example: export JLINK_ROOT=~/SEGGER/JLink_6.40\n"
	exit 1
fi

IMAGE=build/base/bin/embox.bin
SCRIPT=$DIALOG_DA14680_SDK_ROOT/utilities/scripts/qspi/program_qspi_jtag.sh

/bin/bash $SCRIPT --jlink_path $JLINK_ROOT $IMAGE
