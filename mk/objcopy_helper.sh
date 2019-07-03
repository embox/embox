#!/bin/sh

OPTIONS=$1
FILE=$2

OBJCOPY=${EMBOX_CROSS_COMPILE}objcopy

while read ops; do
	$OBJCOPY $ops $FILE
done <$OPTIONS
