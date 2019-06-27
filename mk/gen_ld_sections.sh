#!/bin/sh

# Input
CONFIG_LDS_H=$1
# Output
SECTIONS_LDS_H=$2

rm -f $SECTIONS_LDS_H && touch $SECTIONS_LDS_H

SECTIONS=`grep -o 'LDS_SECTION_VMA_[0-9a-zA-Z_]*' $CONFIG_LDS_H | \
			sed 's/LDS_SECTION_VMA_//g' | uniq`

SECTIONS=`echo $SECTIONS | sed -E 's/\b(text|rodata|data|bss)\b//g'`

for section in $SECTIONS
do
	echo ".$section : ALIGN(DATA_ALIGNMENT) {
	    *(.$section)
	    *(.$section.*)
	} SECTION_REGION($section) :$section" >> $SECTIONS_LDS_H
done
