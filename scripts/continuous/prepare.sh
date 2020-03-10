#!/bin/bash

target=${1//\//__}

x86__test__net() {
	make confload-x86/test/ping-target
	make -j $(nproc)
	cp build/base/bin/embox ./ping-target
}

if ! [ $target ] || ! type -t $target > /dev/null; then
	echo nothing to prepare for \"$1\"
	exit 0
fi
$target "$@"
