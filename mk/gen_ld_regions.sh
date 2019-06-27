#!/bin/sh

# Input
CONFIG_LDS_H=$1
# Output
REGIONS_LDS_H=$2

rm -f $REGIONS_LDS_H && touch $REGIONS_LDS_H

REGIONS=`grep -o 'LDS_REGION_BASE_[0-9a-zA-Z_]*' $CONFIG_LDS_H | \
			sed 's/LDS_REGION_BASE_//g' | sort -u`

for region in $REGIONS
do
	echo "MEMORY_REGION($region)" >> $REGIONS_LDS_H
done
