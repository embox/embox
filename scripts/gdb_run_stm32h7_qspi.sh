#!/usr/bin/env bash

GDB=arm-none-eabi-gdb
GDB_CMD_FILE=$(mktemp --tmpdir=$PWD embox_gdb_stm32h7_qspi.XXXX)
EMBOX_ELF=build/base/bin/embox
EMBOX_BIN=$(mktemp --tmpdir=$PWD embox_XXXX.bin)
EMBOX_BIN0=$(mktemp --tmpdir=$PWD embox_bank0_XXXX.bin)
EMBOX_BIN1=$(mktemp --tmpdir=$PWD embox_bank1_XXXX.bin)
QSPI_BIN=$(mktemp --tmpdir=$PWD qspi_XXXX.bin)

prepare_bin() {
	EMBOX_SECTIONS="--only-section=.text \
			        --only-section=.ARM.exidx \
			        --only-section=.ARM.extab \
			        --only-section=.rodata \
			        --only-section=.data \
			        --only-section=.bss"

	# Assume all other non-standard sections must go to QSPI
	QSPI_SECTIONS="--remove-section=.text \
			       --remove-section=.ARM.exidx \
			       --remove-section=.ARM.extab \
			       --remove-section=.rodata \
			       --remove-section=.data \
			       --remove-section=.bss"

	arm-none-eabi-objcopy -O binary build/base/bin/embox $QSPI_BIN $QSPI_SECTIONS
	arm-none-eabi-objcopy -O binary build/base/bin/embox $EMBOX_BIN $EMBOX_SECTIONS
	dd if=$EMBOX_BIN of=$EMBOX_BIN0 bs=1 count=$((1024 * 1024))
	dd if=$EMBOX_BIN of=$EMBOX_BIN1 bs=1 skip=$((1024 * 1024))	
}

create_gdb_script() {
	rm -f $GDB_CMD_FILE

	echo "target ext :3333" >> $GDB_CMD_FILE
	echo "monitor reset init" >> $GDB_CMD_FILE
	echo "monitor flash banks" >> $GDB_CMD_FILE

	# Write embox.bin
	echo "monitor stm32h7x mass_erase 0" >> $GDB_CMD_FILE
	echo "monitor flash write_bank 0 $EMBOX_BIN0" >> $GDB_CMD_FILE

	echo "monitor stm32h7x mass_erase 1" >> $GDB_CMD_FILE
	echo "monitor flash write_bank 1 $EMBOX_BIN1" >> $GDB_CMD_FILE

	# Write qspi.bin
	sect_size=$(stat -c%s $QSPI_BIN)
	sectors=$((($sect_size + 65535) / 65536)) # Sector size is 64K for stm32h7 QSPI
	echo "monitor flash erase_sector 2 0 $sectors" >> $GDB_CMD_FILE
	echo "monitor flash write_bank 2 $QSPI_BIN" >> $GDB_CMD_FILE
	## Check qspi.bin is written correctly
	echo "monitor flash verify_bank 2 $QSPI_BIN" >> $GDB_CMD_FILE

	echo "monitor reset halt" >> $GDB_CMD_FILE
	echo "continue" >> $GDB_CMD_FILE

	# Show generated GDB sctipt
	echo "GDB script:" && cat $GDB_CMD_FILE && echo ""
}

prepare_bin
create_gdb_script

$GDB -x $GDB_CMD_FILE $EMBOX_ELF

rm $GDB_CMD_FILE
rm $EMBOX_BIN $QSPI_BIN $EMBOX_BIN0 $EMBOX_BIN1
