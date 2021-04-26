#!/usr/bin/env bash

# Input
CONFIG_LDS_H=$1
# Output
SECTION_SYMBOLS_LDS_H=$2
# C arrays of text, data, rodata, bss symbols.
SECTION_ARR_H=$3

rm -f $SECTION_SYMBOLS_LDS_H $SECTION_ARR_H
touch $SECTION_SYMBOLS_LDS_H $SECTION_ARR_H

SECTIONS=$(grep -o 'LDS_SECTION_VMA_[0-9a-zA-Z_]*' $CONFIG_LDS_H | \
			sed 's/LDS_SECTION_VMA_//g' | uniq)

SECTIONS=$(echo $SECTIONS | sed -E 's/\b(text|rodata|data|bss)\b//g')

gensect() {
	name=$1
	decl_file=$(mktemp)
	vma_file=$(mktemp)
	lma_file=$(mktemp)
	len_file=$(mktemp)

	echo "/* $name */" >> $SECTION_SYMBOLS_LDS_H
	echo "/* $name */" >> $SECTION_ARR_H

	echo "void *sections_${name}_vma[] = {" >> $vma_file
	echo "void *sections_${name}_lma[] = {" >> $lma_file
	echo "unsigned int sections_${name}_len[] = {" >> $len_file

	for section in $SECTIONS
	do
		# Match section name. For example, qt_bss, or opencv_text
		if [[ $section =~ _$name$ ]]; then
			echo "SECTION_SYMBOLS($section)" >> $SECTION_SYMBOLS_LDS_H
			echo -e "extern char _${section}_vma, _${section}_lma, _${section}_len;" >> $decl_file
			echo -e "\t&_${section}_vma," >> $vma_file
			echo -e "\t&_${section}_lma," >> $lma_file
			echo -e "\t(unsigned int)&_${section}_len," >> $len_file
		fi
	done

	echo -e "" >> $decl_file
	echo -e "};\n" >> $vma_file
	echo -e "};\n" >> $lma_file
	echo -e "};\n" >> $len_file

	for f in $decl_file $vma_file $lma_file $len_file
	do
		cat $f >> $SECTION_ARR_H
		rm $f
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
