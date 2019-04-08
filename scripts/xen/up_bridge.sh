#!/bin/sh

# dom0: up bridge xenbr0  
sudo ip link add name xenbr0 type bridge
sudo ip addr add dev xenbr0 192.168.2.1/24
sudo ip link set xenbr0 up
bridge link

# Enable routing
sudo sh -c 'echo "1" > /proc/sys/net/ipv4/ip_forward'