#!/bin/bash
script_name=$0
server_name=$1
echo "Input name of server (ex.: www.google.com)"
read server_name
echo "Run $script_name for $server_name"
sudo siege $server_name -t30s
exit 0
