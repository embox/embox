$_MODS += log

$_SRCS-log += log.c

$_DEPS-log += embox.util.LibUtil

$_BRIEF-log = Display last debug messages..

define $_DETAILS-log
NAME
	log - Display last debug messages.
SYNOPSIS
	log [-c cnt]
DESCRIPTION
	Display n last debug messages from common debug log.
AUTHORS
	Timur Abdukadyrov
endef
