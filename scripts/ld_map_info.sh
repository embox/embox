#!/usr/bin/env bash

print_help() {
	echo "Usage:"
	echo -e "./scripts/ld_map_info.sh [MAP_FILE] [SECTION_PATTERN]\n"
	echo "Examples:"
	echo "  ./scripts/ld_map_info.sh build/base/obj/embox-2.o.map \"\.text\""
	echo "  ./scripts/ld_map_info.sh build/base/obj/embox-2.o.map \"\.rodata\""
}

MAP=$1
PATTERN=$2

if [ -z "$MAP" ] || [ -z "$PATTERN" ]; then
	print_help
	exit 1
fi

if awk --non-decimal-data '' &>/dev/null ; then
	AWK='awk --non-decimal-data'
else
	AWK='awk'
fi

export PATTERN_VAR="$PATTERN"

# Short reference on awk to ease understanding the scripts below:
# - awk is controlled by instructions of the form 'pattern { action }'
# - pattern defines when action is performed
# - pattern or action may be omitted, but not both
# - pattern value may be BEGIN, END or <expression>
# - omitted pattern means 'match all'
# - omitted action  means '{ print }'
# - awk splits input lines and stores resulting parts in variables $1, $2, $3, …
#   $0 holds current input line


# $0               current input line
# ~                regular expression matching
# ENVIRON["NAME"]  access to environment variables
# /r/              checks if current input line matches regular expression 'r'
# NF               number of fields obtained after splitting the input line
#
# This script basically does the following: "if current or previous line matches pattern,
# and also current line matches another pattern, then use some fields of this line
# to fill a template and print the result. Always store current line in 'prev' variable."
SELECT_MATCHING_AWK_SCRIPT='
	($0 ~ ENVIRON["PATTERN_VAR"] || prev ~ ENVIRON["PATTERN_VAR"]) && /\.o/ { printf "%-20x%-20d%s\n", $(NF-1), $(NF-1), $NF };
	{ prev=$0 }
'

# BEGIN  matches begining of input
# END    matches end of input
#
# This script prints a line at first, then while consuming input lines it caculates
# some running sum, and at the end it prints another line using calculated sum as
# part of it.
HEADER_FOOTER_AWK_SCRIPT='
	BEGIN { printf "%-20s%-20s%s\n", "Size (hex)", "Size (dec)", "File" }
	{ s += $2; print }
	END { printf "Total: %x (hex) / %d (dec)\n", s, s }
'

$AWK "$SELECT_MATCHING_AWK_SCRIPT" "$MAP" | sort -n -k2 | $AWK "$HEADER_FOOTER_AWK_SCRIPT"
