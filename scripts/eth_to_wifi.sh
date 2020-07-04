#!/bin/sh

set -x

ETH=enp4s0
WIFI=wlp3s0

sudo ifconfig $ETH 192.168.2.1

sudo iptables -t nat -A POSTROUTING -o $ETH -j MASQUERADE
sudo iptables -t nat -A POSTROUTING -o $WIFI -j MASQUERADE
sudo iptables -A FORWARD -i $WIFI -o $ETH -j ACCEPT
sudo iptables -A FORWARD -i $ETH -o $WIFI -j ACCEPT

sudo sysctl net.ipv4.ip_forward=1
