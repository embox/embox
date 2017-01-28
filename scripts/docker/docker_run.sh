#!/usr/bin/env bash

which docker-machine &>/dev/null && eval "$(docker-machine env default)"

container=$(docker ps -q)

if [ ! $container ]; then
	echo "No running container found!"
	exit 1
fi

if [ -t 0 ]; then
	tty_opt="-t"
else
	tty_opt=""
fi

#FIXME Arguments with spaces will break on $*
docker exec -u user -i $tty_opt $container bash -lc "$*"
