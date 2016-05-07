#!/bin/sh

set -v 

docker run --privileged \
	-v $PWD:/embox \
	-P \
	-d embox/emdocker
