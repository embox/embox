
$_MODS += help

$_SRCS-help += help.c
$_USES-help += libframework.a

$_BRIEF-help = Shows all available commands

define $_DETAILS-help
	NAME
		help - displays all possible commands
	SYNOPSIS
		help [-h]
	AUTHORS
		Alexandr Batyukov
endef
