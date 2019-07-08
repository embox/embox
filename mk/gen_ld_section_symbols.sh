#!/bin/sh

# Input
CONFIG_LDS_H=$1
# Output
SECTION_SYMBOLS_LDS_H=$2

rm -f $SECTION_SYMBOLS_LDS_H && touch $SECTION_SYMBOLS_LDS_H

SECTIONS=`grep -o 'LDS_SECTION_VMA_[0-9a-zA-Z_]*' $CONFIG_LDS_H | \
			sed 's/LDS_SECTION_VMA_//g' | uniq`

SECTIONS=`echo $SECTIONS | sed -E 's/\b(text|rodata|data|bss)\b//g'`

for section in $SECTIONS
do
	echo "SECTION_SYMBOLS($section)" >> $SECTION_SYMBOLS_LDS_H
done
