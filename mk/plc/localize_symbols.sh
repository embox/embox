#!/usr/bin/env bash

HELP_MSG="Usage: $ROOT_DIR/mk/plc/localize_symbols.sh <input_file>"

if [ "$#" -ne 1 ]; then
	echo $HELP_MSG
	exit 1
fi

input_file=$1
global_symbols=$input_file.lsyms.txt

objdump=${EMBOX_CROSS_COMPILE}objdump
objcopy=${EMBOX_CROSS_COMPILE}objcopy

$objdump -t $input_file | grep " g " | tr -s ' ' | cut -d \  -f 5 >$global_symbols
echo >>$global_symbols #objcopy dislikes empty files

$objcopy --localize-symbols=$global_symbols $input_file
