#!/bin/sh

SUDO=sudo

TAP=tap78

make confload-x86/test/ping-target
make
cp build/base/bin/embox build/ping-target

$SUDO ip tuntap add dev $TAP mode tap
$SUDO ip link set   dev $TAP address aa:bb:cc:dd:ef:01
$SUDO ip link set   dev $TAP up
$SUDO ip addr flush dev $TAP
$SUDO ip addr add   dev $TAP 192.168.128.1/24 
$SUDO ip addr add   dev $TAP fe80::192:168:128:1/64

export EMBOX_USERMODE_TAP_NAME=$TAP
build/ping-target &

