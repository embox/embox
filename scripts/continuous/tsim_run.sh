#!/bin/sh

OUTPUT_FILE=$1
shift

echo "c" | tsim-leon3 $@ > $OUTPUT_FILE
