#!/bin/sh

SUDO=sudo
ROOT=$(dirname $0)/../.

EMBOX=${ROOT}/build/base/bin/embox

TAP=tap77

rm_tap() {
	$SUDO ip tuntap del dev $TAP mode tap
}

trap rm_tap SIGINT SIGTERM

if [ ! -z "$(ip addr | grep $TAP)" ]; then
	rm_tap
fi

$SUDO ip tuntap add dev $TAP mode tap
$SUDO $ROOT/scripts/qemu/start_script $TAP

if [ $USERMODE_START_OUTPUT ]; then
	$EMBOX $TAP >$USERMODE_START_OUTPUT 2>$USERMODE_START_OUTPUT
else
	$EMBOX $TAP
fi

rm_tap
