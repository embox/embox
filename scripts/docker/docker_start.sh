#!/usr/bin/env bash

dockerimage=$1

# These ports will be forwarded from localhost:(10000 + port) to EMBOX_IP:port
# E.g. localhost:10080 will be forwarded to 10.0.2.16:80
EMBOX_PORTS="80"
EMBOX_IP="10.0.2.16"

localport() {
	echo $(($1 + 10000))
}

expose_ports_args() {
	for port; do
		echo --expose=$port
		echo -p $port:$port
	done
}

docker run -d --privileged \
	--name emdocker \
	-v $PWD:/embox \
	$(expose_ports_args $(localport $EMBOX_PORTS)) \
	${dockerimage:-embox/emdocker}

. $(dirname $0)/docker_rc.sh

# wait till docker started, otherwise getting "No 'user' user found"
waittries=${DOCKER_START_WAIT_TIME_SEC:-10}
started=0
while [ 0 -lt $waittries ]; do
	if dr true 2>/dev/null 1>/dev/null; then
		started=1
		break;
	fi
	sleep 1
	waittries=$(($waittries - 1))
done

if [ x0 = x$started ]; then
	echo "Container is not stated for some time, trying one more time in verbose mode" >&2
	echo "If it fail, try to change timeout specifying DOCKER_START_WAIT_TIME_SEC env, like" >&2
	echo "" >&2
	echo "$ DOCKER_START_WAIT_TIME_SEC=60 $0 $*" >&2
	dr true && started=1
fi

if [ x0 = x$started ]; then
	echo "Can't start container" >&2
	exit 1
fi

# sometime we can get into container before it's fully initialized
# this artifical delay helps to prevent this
sleep 0.5

for port in $EMBOX_PORTS; do
	dr sudo iptables -A PREROUTING -t nat -p tcp --dport $(localport $port) -j DNAT --to $EMBOX_IP:$port
done

dr "echo export PATH=/embox/scripts/docker/container/bin:\$PATH >> /home/user/.bashrc"
