#!/usr/bin/env bash

case " $@ " in
	*" -shared "* | \
	*" -pthread "* | \
	*" -rdynamic "* | \
	*" -l"* | \
	*" -I/usr/"*)
		$0 $(echo " $@" | sed \
		-e 's/ -shared//g' \
		-e 's/ -pthread//g' \
		-e 's/ -rdynamic//g' \
		-e 's/ -l[0-9a-zA-Z_]*//g' \
		-e 's/ -I\/usr\/[0-9a-zA-Z_]*//g'); \
		exit $?;;
esac

: "${EMBOX_DIST_BASE_DIR:=$(realpath $(dirname $(realpath $0))/../base)}"

