#!/usr/bin/env bash

print_help() {
	echo "Usage:"
	echo -e "./scripts/ld_map_info.sh [MAP_FILE] [SECTION_PATTERN]\n"
	echo "Examples:"
	echo "  ./scripts/ld_map_info.sh build/base/obj/embox-2.o.map \"\.text\""
	echo "  ./scripts/ld_map_info.sh build/base/obj/embox.o.map \"\.rodata\""
}

MAP=$1
PATTERN=$2

if [ -z $MAP ] || [ -z $PATTERN ]; then
	print_help
	exit 1
fi

cat $MAP | \
	awk -v pat="$PATTERN" '($0 ~ pat || prev ~ pat) && /\.o/; { prev=$0 }' | \
	awk '{ printf "%-20x%-20d%s\n", $(NF-1), $(NF-1), $NF }' | sort -n -k2 | \
	awk 'BEGIN { printf "%-20s%-20s%s\n", "Size (hex)", "Size (dec)", "File" }
	     { s += $2; print }
	     END { printf "Total: %x (hex) / %d (dec)\n", s, s }'
