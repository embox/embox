$_MODS += man
$_SRCS-man += man.c
$_USES-man += libframework.a

$_BRIEF-man = Shows the built-in command reference

define $_DETAILS-man
	NAME
		man - an interface to the built-in command reference
	SYNOPSIS
		man cmd ...
	DESCRIPTION
		man displays the built-in manual pages for the specified commands
	AUTHORS
		Nikolay Korotky
endef
