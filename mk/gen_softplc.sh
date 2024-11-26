#!/usr/bin/env bash

HELP_MSG="Usage: $ROOT_DIR/mk/gen_softplc.sh <input_file> <output_file>"

if [ "$#" -ne 2 ]; then
	echo $HELP_MSG
	echo "error: Invalid number of arguments"
	exit 1
fi

if [ -z "$IEC2C" ]; then
	echo $HELP_MSG
	echo "error: IEC2C variable not found"
	exit 1
fi

if [ -z "$IECLIB" ]; then
	echo $HELP_MSG
	echo "error: IECLIB variable not found"
	exit 1
fi

if [ -z "$PLC_MAIN" ]; then
	echo $HELP_MSG
	echo "error: PLC_MAIN variable not found"
	exit 1
fi

input_file=$1
output_file=$2

# Create tmp direcory
tmp_dir=$(mktemp -d /tmp/softplc-XXXXX)

# Generate SoftPLC in tmp directory
$IEC2C -f -l -p -I $IECLIB -T $tmp_dir $input_file || {
	rm -rf $tmp_dir # Remove tmp direcory
	echo $HELP_MSG
	echo "error: SoftPLC generation failed"
	exit 1
}

# Check name of configuration
if [ ! -f $tmp_dir/config.h ]; then
	rm -rf $tmp_dir # Remove tmp direcory
	echo $HELP_MSG
	echo "error: CONFIGURATION 'config' not found"
	exit 1
fi

# Generated SoftPLC files
softplc_files="$tmp_dir/POUS.h $tmp_dir/config.h $(find $tmp_dir -name \*.c)"

# Located variables in one line
located_vars=$(sed '$!N;s/\n/ /' <$tmp_dir/LOCATED_VARIABLES.h)

# Write SoftPLC code to output file
printf "#define __LOCATED_VAR_LIST $located_vars\n\n" >$output_file
printf "#include \"$PLC_MAIN\"\n" >>$output_file
for file in $softplc_files; do
	printf "\n\n" >>$output_file
	cat $file >>$output_file
done

# Remove inclusion of POUS.c, POUS.h and config.h
sed -i '/#include "POUS.c"/d' $output_file
sed -i '/#include "POUS.h"/d' $output_file
sed -i '/#include "config.h"/d' $output_file

# Remove tmp direcory
rm -rf $tmp_dir
