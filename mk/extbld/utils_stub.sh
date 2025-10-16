#!/usr/bin/env bash

cmd=$(basename $0)
cmd=${cmd#embox-}

${EMBOX_CROSS_COMPILE}${cmd} "$@"
exit $?
