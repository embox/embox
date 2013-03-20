#!/bin/sh

VISOR=./emvisor
EMBOX=./build/base/bin/embox

PDOWNSTRM=fdownstream
PUPSTRM=fupstream

trap 'echo Exiting; kill $VISORPID' SIGINT SIGTERM

$VISOR $PDOWNSTRM $PUPSTRM &
VISORPID=$!
$EMBOX $PDOWNSTRM $PUPSTRM
