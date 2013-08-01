#!/bin/sh

ROOT=$(dirname $0)/../.

VISOR=./emvisor
EMBOX=${ROOT}/build/base/bin/embox
TAP=tap77

EMVISOR_MK=${ROOT}/src/arch/usermode86/emvisor.mk

PDOWNSTRM=fdownstream
PUPSTRM=fupstream

THISUSER=$USER

[ -p $PDOWNSTRM ] || mkfifo $PDOWNSTRM
[ -p $PUPSTRM ] || mkfifo $PUPSTRM

if [ ! -z "$(ip addr | grep $TAP)" ]; then
	sudo ip tuntap del dev $TAP mode tap
fi

sudo ip tuntap add dev $TAP mode tap user $THISUSER
sudo ip addr change 10.0.2.10 dev $TAP
sudo ip link set $TAP up
sudo ip route add 10.0.2.0/24 dev $TAP

[ -x $VISOR ] || make -f $EMVISOR_MK

$VISOR $PDOWNSTRM $PUPSTRM $EMBOX

