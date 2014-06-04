#!/bin/sh

PROGNAME=$1
shift

cd `dirname $0`
$PROGNAME "$@"
