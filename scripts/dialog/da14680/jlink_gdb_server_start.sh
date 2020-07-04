#!/bin/sh

if [ -z $JLINK_ROOT ]; then
	printf "JLINK_ROOT is not set. Please specify the path.\n"
	printf "  Example: export JLINK_ROOT=~/SEGGER/JLink_6.40\n"
	exit 1
fi

$JLINK_ROOT/JLinkGDBServerCLExe \
	-device DA14680 -if SWD -speed 8000 \
	-port 2331 -swoport 2332 -telnetport 2333 -singlerun $@
