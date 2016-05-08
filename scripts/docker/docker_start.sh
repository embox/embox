#!/bin/sh

# These ports will be forwarded from localhost:(10000 + port) to EMBOX_IP:port
# E.g. localhost:10080 will be forwarded to 10.0.2.16:80
EMBOX_PORTS="80"
EMBOX_IP="10.0.2.16"

dr=$(dirname $0)/docker_run.sh

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
	-v $PWD:/embox \
	$(expose_ports_args $(localport $EMBOX_PORTS)) \
	embox/emdocker

# wait till docker started, otherwise getting "No 'user' user found"
until $dr true; do
	sleep 0.1
done

for port in $EMBOX_PORTS; do
	$dr sudo iptables -A PREROUTING -t nat --dport $(localport $port) -j DNAT --to $EMBOX_IP:$port
done
