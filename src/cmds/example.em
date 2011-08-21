$_MODS += example
$_SRCS-example += example.c
$_USES-example += libframework.a

$_BRIEF-example = Interacts with Embox example framework

define $_DETAILS-example
	NAME
		example - an interface to Embox examples execution
	SYNOPSIS
		test [-h] <example_name>
	DESCRIPTION
		Shows example available in the system and executes the specified ones
	OPTIONS
		If no option is specified then the command shows the list of available
		examples.
	AUTHORS
		Anton Bondarev
endef
