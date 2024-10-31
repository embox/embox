#!/usr/bin/env bash

HELP_MSG="Usage: ./scripts/beremiz.sh <path_to_beremiz>"

EMBOX_FB=$(pwd)/build/extbld/project/softplc/iecsup/install/lib/embox_ieclib.iecst

if [ "$#" -ne 1 ]; then
	echo $HELP_MSG
	echo "error: Invalid number of arguments"
	exit 1
fi

if [ ! -f $1/Beremiz.py ] || [ ! -f $1/ImportEmboxFB.py ]; then
	echo $HELP_MSG
	echo "error: Invalid path to beremiz"
	exit 1
fi

if [ ! -f $1/.venv/bin/python ]; then
	echo $HELP_MSG
	echo "error: $1/.venv/bin/python not found"
	exit 1
fi

if [ -f $EMBOX_FB ]; then
	$1/.venv/bin/python $1/ImportEmboxFB.py $EMBOX_FB
else	
	TMPFILE=$(mktemp /tmp/ieclib-XXXXX)
	$1/.venv/bin/python $1/ImportEmboxFB.py $TMPFILE
	rm -f $TMPFILE
fi

$1/.venv/bin/python $1/Beremiz.py
