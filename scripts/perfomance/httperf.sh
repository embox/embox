#!/usr/bin/env bash
#
# Network performance test with `httperf' utillity
# Author: Anastasia Novoselova

script_name=$0;
server_name=$1;
echo "Input name of server (ex.: google.com)"
read server_name
echo "Run $script_name for $server_name"
httperf --server $server_name
exit 0
