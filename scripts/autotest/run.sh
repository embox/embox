#!/bin/bash

# - BASH_SOURCE is an array variable whose members are source filenames.
# - '%/*' — is a parameter expansion feature which uses everything after
#   %-sign as a pattern and removes the shortest match to this pattern
#   from the parameter.
# This magic spell gives a path to a dir where current source file
# is located.
BASEDIR=$(realpath ${BASH_SOURCE%/*})
TESTSUITE_DIR=$BASEDIR/testsuite


print_usage() {
	echo "Usage: $0 <testsuite> <test case 1> <test case 2> …"
	echo "  You can 'export TEST_PRINT_ALL=1' to make tests echo to console"

}


# Prints available testsuites
# Usage:
#    show_available_testsuites
#
# This function depends on a variable $TESTSUITE_DIR which is defined
# in global scope.
show_available_testsuites() {
	echo "Available testsuites:"
	ls "$TESTSUITE_DIR"
}


# Prints testcases of the testsuite passed to function
#
# Usage:
#    show_available_testcases <testsuite name>, e.g.:
#      show_available_testcases block_dev
#      show_available_testcases fs-ramfs
#      show_available_testcases "$TESTSUITE_NAME"
#      ...
#
# This function depends on a variable $TESTSUITE_DIR which is defined
# in global scope.
show_available_testcases() {
	local TESTSUITE=$1
	echo "Availables testcase for testsuite '$TESTSUITE':"

	# Print testcase names on one line separated with spaces.
	# '-n' omits trailing newline
	for testcase in "$TESTSUITE_DIR/$TESTSUITE"/*/; do
	    echo -n "$(basename "$testcase") "
	done
	echo
}


if [ "$1" = "-h" ]; then
	print_usage
	exit 1
fi


# Check what caller has passed as a test suite name
TESTSUITE=$1
if [ -z "$TESTSUITE" ]; then
	echo 'You have to select a test suite!'
	echo
	show_available_testsuites
	echo
	print_usage
	exit 1
fi

if [ ! -d "$TESTSUITE_DIR/$TESTSUITE" ]; then
	echo "'$TESTSUITE' is not a valid test suite name!"
	echo
	show_available_testsuites
	exit 1
fi


# At this point we've successfully checked that the first argument to this script
# was a valid testsuite name. We've saved it in $TESTSUITE variable.
#
# Rest of the args are considered to be testcase names. We want to store them in an array.
# To do this:
# 1. Shift arguments to the left by one to strip the first argument
# 2. Assign values of $@ array to a new array
shift
TESTCASES=( "$@" )


# Check if a caller has supplied at least one testcase name
if [[ "${#TESTCASES[@]}" -eq 0 ]]; then
	echo "You have to select at least one testcase!"
	echo
	show_available_testcases "$TESTSUITE"
	echo
	print_usage
	exit 1
fi

# Check if every value in TESTCASES array is a valid testcase name.
#
# Test definitions are organazed on disk in hierarchy of directories:
#   ./testsuite/<testsuite name>/<testcase name>/
#
# If such a path exists, then testsuite and testcase names are correct.
for testcase in "${TESTCASES[@]}"; do
	if [ ! -d "$TESTSUITE_DIR/$TESTSUITE/$testcase" ]; then
		echo "Test '$testcase' is not part of testsuite '$TESTSUITE'"
		echo
		show_available_testcases "$TESTSUITE"
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

printf '\nCurrent configuration: "%s"\n' "$TEST_CURRENT_CONFIG"

if [ -z "$TEST_PRINT_ALL" ]; then
	TEST_PRINT_ALL=1
fi

echo "Starting testsuite $TESTSUITE ..."

rm -f "$TESTSUITE.log"

testsuite_res=0
summary=
for testcase in "${TESTCASES[@]}"; do
	echo "  Starting test $TESTSUITE/$testcase ..."

	if [ "$TEST_PRINT_ALL" -eq 0 ]; then
		expect "$BASEDIR/framework/core.exp" "$BASEDIR/testsuite/$TESTSUITE" "$testcase" \
			$EMBOX_IP $HOST_IP "$EMBOX_PROMPT" > "${TESTSUITE}_$testcase.log"
	else
		expect "$BASEDIR/framework/core.exp" "$BASEDIR/testsuite/$TESTSUITE" "$testcase" \
			$EMBOX_IP $HOST_IP "$EMBOX_PROMPT"
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
