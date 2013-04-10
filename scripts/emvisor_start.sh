#!/bin/sh

VISOR=./emvisor
EMBOX=./build/base/bin/embox
TAP=tap77

PDOWNSTRM=fdownstream
PUPSTRM=fupstream

THISUSER=$USER

[ -p $PDOWNSTRM ] || mkfifo $PDOWNSTRM
[ -p $PUPSTRM ] || mkfifo $PUPSTRM

if [ -z "$(ip addr | grep $TAP)" ]; then
	sudo ip tuntap add dev $TAP mode tap user $THISUSER
	sudo ip addr change 10.0.2.10 dev $TAP
	sudo ip link set $TAP up
	sudo ip route add 10.0.2.0/24 dev $TAP
fi

$VISOR $PDOWNSTRM $PUPSTRM $EMBOX
