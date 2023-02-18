#!/usr/bin/env bash

grep 'QSPI)' ./conf/lds.conf > /dev/null && echo "This script doesn't work with QSPI sections!" && exit -1

GDB=arm-none-eabi-gdb
GDB_CMD_FILE=$(mktemp --tmpdir=$PWD embox_gdb_stm32f7_qspi.XXXX)
EMBOX_ELF=build/base/bin/embox
EMBOX_BIN=$(mktemp --tmpdir=$PWD embox_XXXX.bin)

create_gdb_script() {
	rm -f $GDB_CMD_FILE

	echo "target ext :3333" >> $GDB_CMD_FILE
	echo "monitor reset halt" >> $GDB_CMD_FILE
	echo "load" >> $GDB_CMD_FILE
	echo "continue" >> $GDB_CMD_FILE

	# Show generated GDB sctipt
	echo "GDB script:" && cat $GDB_CMD_FILE && echo ""
}

create_gdb_script

$GDB -x $GDB_CMD_FILE $EMBOX_ELF

rm $GDB_CMD_FILE
