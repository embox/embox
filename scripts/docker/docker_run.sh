#!/bin/bash

which docker-machine &>/dev/null && eval "$(docker-machine env default)"

container=$(docker ps -q)

if [ ! $container ]; then
	echo "No running container found!"
	exit 1
fi

#FIXME Arguments with spaces will break on $*
docker exec -u user -it $container bash -lc "$*"
