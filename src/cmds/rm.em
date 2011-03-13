$_MODS += rm
$_SRCS-rm += rm.c
$_DEPS-rm += embox.fs.core

$_BRIEF-rm = Remove file or directory

define $_DETAILS-rm
	NAME
		rm - remove file or directory
	SYNOPSIS
		rm [OPTIONS] FILE
	DESCRIPTION
		rm removes each specified file
	OPTIONS
		-r - remove directories and their contents recursively
		-f - ignore nonexistent files, never prompt
	AUTHOR
		Roman Evstifeev
endef
