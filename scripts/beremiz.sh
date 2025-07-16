#!/usr/bin/env bash

HELP_MSG="Usage: ./scripts/beremiz.sh <path_to_beremiz>"

EMBOX_FB=./build/base/gen/embox_ieclib.iecst

if [ "$#" -ne 1 ]; then
	echo $HELP_MSG
	echo "error: Invalid number of arguments"
	exit 1
fi

if [ ! -f $(pwd)/scripts/beremiz.sh ]; then
	echo $HELP_MSG
	echo "error: beremiz.sh must be run from the root directory of Embox"
	exit 1
fi

if [ ! -f $1/Beremiz.py ] || [ ! -f $1/ImportEmboxFB.py ]; then
	echo $HELP_MSG
	echo "error: Invalid path to Beremiz"
	exit 1
fi

if [ ! -f $1/.venv/bin/python ]; then
	echo $HELP_MSG
	echo "error: $1/.venv/bin/python not found"
	exit 1
fi

printf "\n>>> Generating Embox function blocks for Beremiz <<<\n\n"
make -j$(nproc) buildgen &> /dev/null
echo > $EMBOX_FB

if [ -d ./build/base/include/matiec_lib ]; then
	find ./build/base/include/matiec_lib -name \*.iecst \
		-exec cat {} >> $EMBOX_FB \; \
		-exec printf "\n\n" >> $EMBOX_FB \;
fi

$1/.venv/bin/python $1/ImportEmboxFB.py $EMBOX_FB

printf "\n>>> Launch Beremiz <<<\n\n"
$1/.venv/bin/python $1/Beremiz.py
