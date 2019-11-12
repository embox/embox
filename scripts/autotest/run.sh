#!/bin/bash

BASEDIR=$(realpath ${BASH_SOURCE%/*})
TESTSUITE_DIR=$BASEDIR/testsuite
TESTSUITE_LIST=$(ls $TESTSUITE_DIR)

TESTCASE_DIR=
TESTCASE_LIST=

show_available_testsuites() {
	echo "Available testsuites:"
	echo $TESTSUITE_LIST
}

show_available_testcases() {
	echo "Availables testcase for testsuite $TESTSUITE:"
	echo $TESTCASE_LIST
}

if [ "$1" = "-h" ]; then
	echo "Usage: $0 <testsuite> <test cases>"
	echo "  You can 'export TEST_PRINT_ALL=1' to make tests echo to console"
	exit 1
fi

TESTSUITE=$1

if [ -z $TESTSUITE ]; then
	show_available_testsuites
	exit 1
fi

TESTCASE_DIR=$BASEDIR/testsuite/$TESTSUITE
TESTCASE_LIST=$(for i in $(ls -d $TESTCASE_DIR/*/); do basename $i; done)

shift
TESTCASES=$@

if [ -z "$TESTCASES" ]; then
	show_available_testcases
	exit 1
fi

for testcase in $TESTCASES; do
	if [ -z "$(echo $TESTCASE_LIST | fgrep -w $testcase)" ]; then
		echo "Test '$testcase' is not part of testsuite '$TESTSUITE'"
		exit 1
	fi
done

if [ -z "$TEST_EMBOX_ROOT" ]; then
	if [ "$EUID" -eq 0 ]; then
		echo "Please, run with 'sudo -E'"
		exit 1
	fi
	echo "TEST_EMBOX_ROOT is not set. 'export TEST_EMBOX_ROOT=<path to embox>'"
	exit 1
fi

if [ -z "$TEST_CURRENT_CONFIG" ]; then
	echo "Configuration is not set"
	echo "Please, source one of scripts/autotest/configs/*"
	echo " E.g.: source scripts/autotest/configs/default.sh, for QEMU default"
	exit 1
fi

printf "\nCurrent configuration: \"$TEST_CURRENT_CONFIG\"\n"

if [ -z "$TEST_PRINT_ALL" ]; then
	TEST_PRINT_ALL=1
fi

echo "Starting testsuite $TESTSUITE ..."

rm -f $TESTSUITE.log

testsuite_res=0
summary=
for testcase in $TESTCASES; do
	echo "  Starting test $TESTSUITE/$testcase ..."

	if [ ${TEST_PRINT_ALL} -eq 0 ]; then
		expect $BASEDIR/framework/core.exp $BASEDIR/testsuite/$TESTSUITE $testcase \
			$EMBOX_IP $HOST_IP $EMBOX_PROMPT > ${TESTSUITE}_$testcase.log
	else
		expect $BASEDIR/framework/core.exp $BASEDIR/testsuite/$TESTSUITE $testcase \
			$EMBOX_IP $HOST_IP $EMBOX_PROMPT
	fi
	rc=$?

	result="$TESTSUITE/$testcase:"

	if [ $rc -ne 0 ]; then
		result="$result FAILED"
		testsuite_res=1
	else
		result="$result PASSED"
	fi

	echo "  $result"
	summary="$summary$result\n"
done

if [ $testsuite_res -eq 0 ]; then
	status="OK"
else
	status="ERROR"
fi

echo "Test summary:"
printf "$summary\n"
echo "Testsuite $TESTSUITE finished. STATUS = $status"

exit ${testsuite_res}
