#!/bin/bash
#
# Network perfomance test with `ab' utillity
# Author: Anastasia Novoselova

script_name=$0;
server_name=$1;
echo "Input name of server in format: [http[s]://]hostname[:port]/path
(ex.:http://google.com/)"
read server_name
echo "Run $script_name for $server_name..."
ab $server_name
exit 0
