#!/bin/sh

# Input
CONFIG_LDS_H=$1
# Output
PHDRS_LDS_H=$2

rm -f $PHDRS_LDS_H && touch $PHDRS_LDS_H

SECTIONS=`grep -o 'LDS_SECTION_PHDR_TYPE_[0-9a-zA-Z_]*' $CONFIG_LDS_H | \
			sed 's/LDS_SECTION_PHDR_TYPE_//g'`

for section in $SECTIONS
do
	phdr_type=`grep LDS_SECTION_PHDR_TYPE_$section $CONFIG_LDS_H | awk '{print $3}'`
	phdr_flags=`grep LDS_SECTION_PHDR_FLAGS_$section $CONFIG_LDS_H | awk '{print $3}'`
	echo "$section $phdr_type $phdr_flags;" >> $PHDRS_LDS_H
done
