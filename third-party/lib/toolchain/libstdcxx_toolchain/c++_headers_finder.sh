#!/bin/sh

AWK=$1

${EMBOX_CROSS_COMPILE}gcc -Wp,-v -x c++ /dev/null -fsyntax-only 2>&1 >/dev/null | \
	$AWK 'BEGIN { ORS = " " }; \
	      /End of search list/ { found = 0 }; \
	      found && /c\+\+/ {print "-I" $1}; \
	      /#include <\.\.\.> search starts here/ { found = 1 }'
