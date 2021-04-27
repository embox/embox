#!/usr/bin/env bash

# Input
CONFIG_LDS_H=build/base/gen/config.lds.h

SECTIONS=$(grep -o 'LDS_SECTION_VMA_[0-9a-zA-Z_]*' $CONFIG_LDS_H | \
			sed 's/LDS_SECTION_VMA_//g' | uniq)

SECTIONS=$(echo $SECTIONS | sed -E 's/\b(text|rodata|data|bss)\b//g')

gensect() {
	name=$1
	decl_file=$(mktemp)
	vma_file=$(mktemp)
	lma_file=$(mktemp)
	len_file=$(mktemp)

	echo "/* $name */"

	echo "void *sections_${name}_vma[] = {" >> $vma_file
	echo "void *sections_${name}_lma[] = {" >> $lma_file
	echo "unsigned int sections_${name}_len[] = {" >> $len_file

	for section in $SECTIONS
	do
		# Match section name. For example, qt_bss, or opencv_text
		if [[ $section =~ _$name$ ]]; then
			echo -e "extern char _${section}_vma, _${section}_lma, _${section}_len;" >> $decl_file
			echo -e "\t&_${section}_vma," >> $vma_file
			echo -e "\t&_${section}_lma," >> $lma_file
			echo -e "\t(unsigned int)&_${section}_len," >> $len_file
		fi
	done

	echo -e "" >> $decl_file

	echo -e "\t(void *)0," >> $vma_file
	echo -e "\t(void *)0," >> $lma_file
	echo -e "\t(unsigned int)-1," >> $len_file

	for f in $vma_file $lma_file $len_file
	do
		echo -e "};\n" >> $f
	done

	for f in $decl_file $vma_file $lma_file $len_file
	do
		cat $f
		rm $f
	done
}

gensect text
gensect rodata
gensect data
gensect bss
