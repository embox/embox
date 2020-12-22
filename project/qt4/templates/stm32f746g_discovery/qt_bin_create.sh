#!/bin/sh

set -e

# Change to your location. It's usually either /srv/tftp or /var/lib/tftpboot.
TFTP_FOLDER=/srv/tftp

QT_BIN=qt.bin
EMBOX_QT_ELF=embox_qt.elf

rm -f $QT_BIN $EMBOX_QT_ELF

arm-none-eabi-objcopy -O binary build/base/bin/embox $QT_BIN \
	--only-section=.qt_text --only-section=.qt_rodata --only-section=.qt_data

arm-none-eabi-objcopy build/base/bin/embox $EMBOX_QT_ELF \
	--remove-section=.qt_text --remove-section=.qt_rodata \
	--remove-section=.qt_data --remove-section=.qt_bss

sudo cp qt.bin $TFTP_FOLDER

echo "$QT_BIN successfully copied to $TFTP_FOLDER. Load it to QSPI now."
echo "Then flash and run $EMBOX_QT_ELF as usual."
