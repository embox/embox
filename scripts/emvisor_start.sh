#!/bin/sh

VISOR=./emvisor
EMBOX=./build/base/bin/embox

PDOWNSTRM=fdownstream
PUPSTRM=fupstream

trap 'echo Exiting; kill $EMBOXPID' SIGINT SIGTERM

$EMBOX $PDOWNSTRM $PUPSTRM &
EMBOXPID=$!
$VISOR $EMBOXPID $PDOWNSTRM $PUPSTRM
