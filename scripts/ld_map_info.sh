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

if awk --non-decimal-data '' &>/dev/null ; then
	AWK='awk --non-decimal-data'
else
	AWK='awk'
fi

export PATTERN_VAR="$PATTERN"

cat $MAP | \
	$AWK '($0 ~ ENVIRON["PATTERN_VAR"] || prev ~ ENVIRON["PATTERN_VAR"]) && /\.o/ \
	     { printf "%-20x%-20d%s\n", $(NF-1), $(NF-1), $NF }; { prev=$0 }' | sort -n -k2 | \
	$AWK 'BEGIN { printf "%-20s%-20s%s\n", "Size (hex)", "Size (dec)", "File" }
	     { s += $2; print }
	     END { printf "Total: %x (hex) / %d (dec)\n", s, s }'
