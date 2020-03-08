#!/bin/bash

# Since travis_wait doesn't work with docker well somewhy,
# just use bell which prints something to stdout every 5 minutes.
function bell() {
	i=0
	while true :
	do
		sleep 5m
		i=$(($i+5))
		echo "CI: Still building (minutes=$i)"
	done
}

bell &
bell_pid=$!

make -j $(nproc) &> build.log
RETVAL=$?
tail -c 3M build.log

# https://github.com/travis-ci/travis-ci/issues/6018 -- Prevert log truncation
# on error. There are also many issues which refer to this one.
sleep 5

kill $bell_pid
exit $RETVAL
