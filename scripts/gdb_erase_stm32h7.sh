#!/usr/bin/env bash

GDB=arm-none-eabi-gdb
GDB_CMD_FILE=$(mktemp --tmpdir="$PWD" embox_gdb_stm32.XXXX)

create_gdb_script() {
	rm -f "$GDB_CMD_FILE"

	{
	    echo "target ext :3333"
	    echo "monitor reset halt"
	    echo "monitor flash banks"

	    # Erase sector flash0 and flash1
	    echo "monitor stm32h7x mass_erase 0"
	    echo "monitor stm32h7x mass_erase 1"

	} >> "$GDB_CMD_FILE"

	# Show generated GDB sctipt
	echo "GDB script:" && cat "$GDB_CMD_FILE" && echo ""
}

create_gdb_script

$GDB -batch -x "$GDB_CMD_FILE"
rm -f "$GDB_CMD_FILE"
