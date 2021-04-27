#!/usr/bin/env bash

# Input
CONFIG_LDS_H=$1
# Output
SECTION_SYMBOLS_LDS_H=$2

rm -f $SECTION_SYMBOLS_LDS_H
touch $SECTION_SYMBOLS_LDS_H

SECTIONS=$(grep -o 'LDS_SECTION_VMA_[0-9a-zA-Z_]*' $CONFIG_LDS_H | \
			sed 's/LDS_SECTION_VMA_//g' | uniq)

SECTIONS=$(echo $SECTIONS | sed -E 's/\b(text|rodata|data|bss)\b//g')

gensect() {
	name=$1

	echo "/* $name */" >> $SECTION_SYMBOLS_LDS_H

	for section in $SECTIONS
	do
		# Match section name. For example, qt_bss, or opencv_text
		if [[ $section =~ _$name$ ]]; then
			echo "SECTION_SYMBOLS($section)" >> $SECTION_SYMBOLS_LDS_H
		fi
	done
}

gensect text
gensect rodata
gensect data
gensect bss

echo "/* Other sections (not text, rodata, data or bss) */" >> $SECTION_SYMBOLS_LDS_H
for section in $SECTIONS
do
	if ! [[ $section =~ (_text|_data|_rodata|_bss)$ ]]; then
		echo "SECTION_SYMBOLS($section)" >> $SECTION_SYMBOLS_LDS_H
	fi
done
