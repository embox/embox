#!/bin/bash
host=${DOCKER_HOST:-localhost}
port=${DOCKER_SSH_PORT:-22222}

ssh -tti $(dirname $0)/docker.id_rsa user@${host} -p ${port} "$@"
