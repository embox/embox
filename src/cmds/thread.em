
$_MODS += thread

$_SRCS-thread += thread.c

$_DEPS-thread +=

$_BRIEF-thread = Works with threading subsystem

define $_DETAILS-thread
	NAME
		thread - Works with threading subsystem
	SYNOPSIS
		thread [-h] [-s] [-k <id>]
	DESCRIPTION
		thread
		thread -h
			Prints usage
		thread -s
			Prints threads statistics
		thread -k <is>
			Stops the thread with the specified id
	AUTHORS
		Gleb Efimov, Alina Kramar, Roman Oderov
endef
