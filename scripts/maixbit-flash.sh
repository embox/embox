#!/usr/bin/env bash
# @file
# @brief Load firmware to MAiX BiT board
#		Requires kflash.py(https://github.com/kendryte/kflash.py)
# @author	sksat
# @date		05.08.2020
#

BUILD_DIR=./build/base/bin
EM_BIN=$BUILD_DIR/embox.bin

K210_PORT=/dev/ttyUSB0

usage() {
	echo "Load firmware to MAiX BiT board."
	echo
	echo "Usage:"
	echo "    $0 <K210_PORT>"
}

if [ $# -ne 0 ]; then
	case $1 in
		-h)	usage; exit -1 ;;
		*) K210_PORT=$1 ;;
	esac
fi

if [ ! -e $K210_PORT ]; then
	echo 'K210_PORT does not found. Please specify K210_PORT manually.'
	exit -1
fi

kflash -p $K210_PORT $EM_BIN
