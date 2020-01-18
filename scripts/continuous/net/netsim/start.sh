#!/bin/sh

set -x

IP=$1

B=$(dirname $0)
BUILD=./build
NETSIM=$BUILD/netsim

SUDO=
if [ $(id -u) != 0 ]; then
	SUDO=sudo
fi

mkdir -p $BUILD
if ! [ -f $NETSIM ]; then
	gcc -o $NETSIM $B/netsim.c
fi

$SUDO ip route add $IP dev lo
exec $SUDO $NETSIM -l $IP
