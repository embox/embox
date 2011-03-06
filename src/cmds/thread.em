
$_MODS += thread

$_SRCS-thread += thread.c

$_DEPS-thread +=

$_BRIEF-thread = Works with threading subsystem

define $_DETAILS-thread
	NAME
		thread - interacts with threading subsystem
	SYNOPSIS
		thread [-h] [-s] [-k id]
	OPTIONS
		-h
			Prints usage
		-s
			Prints threads statistics
		-k id
			Stops the thread with the specified id
	AUTHORS
		Gleb Efimov, Alina Kramar, Roman Oderov
endef
