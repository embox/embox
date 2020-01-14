#!/bin/sh

IP=$1

B=$(dirname $0)
NETSIM=./build/netsim

SUDO=
if [ $(id -u) != 0 ]; then
	SUDO=sudo
fi

if ! [ -f $NETSIM ]; then
	gcc -o $NETSIM $B/netsim.c
fi

$SUDO ip route add $IP dev lo
$SUDO $NETSIM -l $IP &
