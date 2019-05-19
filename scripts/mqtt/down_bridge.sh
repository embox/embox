#!/bin/bash
ip=$(which ip)

$ip link delete br0 type bridge
