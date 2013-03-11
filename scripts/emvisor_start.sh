#!/bin/sh

VISOR=./emvisor
EMBOX=./build/base/bin/embox

PDOWNSTRM=fdownstream
PUPSTRM=fupstream

$EMBOX $PDOWNSTRM $PUPSTRM &
$VISOR $! $PDOWNSTRM $PUPSTRM
