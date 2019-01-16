#!/usr/bin/env bash

ROOT_DIR=.
BASE_DIR=$ROOT_DIR
CONT_BASE=$ROOT_DIR/scripts/continuous
QT_SCRIPTS_DIR=$CONT_BASE/qt
PID_FILE=$BASE_DIR/qemu_bg.pid
CONT_RUN=$CONT_BASE/run.sh
OUTPUT_FILE=./cont.out

final_ret=0
check_post_exit() {
	ret=$?
	if [ 0 -ne $ret ]; then
		echo - - - - - - - - - - - - - - -
		echo ERROR: $1
		echo - - - - - - - - - - - - - - -

		final_ret=$ret
	fi
}

export CONTINIOUS_RUN_TIMEOUT=80
$CONT_RUN generic/qemu_bg_no_check "" $PID_FILE

# Check if Qt started
grep "QVNCServer created" $OUTPUT_FILE &>/dev/null
ret=$?
if [ $ret -ne 0 ]; then
	echo "Error: Qt wasn't started"
	exit 1
fi

# Run test_run.sh inside new display :100
export QT_VNC_DISPLAY_NR=100
xvfb-run -nocursor --server-args="-screen 0 1024x768x24" \
	-n $QT_VNC_DISPLAY_NR $QT_SCRIPTS_DIR/test_run.sh

check_post_exit "Test Qt moveblocks over vnc FAILED"

$CONT_RUN generic/qemu_bg_kill "" $PID_FILE
rm $PID_FILE

if [ $final_ret -eq 0 ]; then
	echo "Test Qt moveblocks over vnc PASSED"
fi

exit $final_ret
