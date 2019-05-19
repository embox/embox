 #!/bin/bash
ip=$(which ip)

$ip link add name br0 type bridge
$ip link set dev br0 up
$ip addr flush dev br0
$ip link set dev br0 address aa:bb:cc:dd:ee:b0
$ip addr add dev br0 10.0.2.40/24
