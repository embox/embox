$_MODS += runelf
$_SRCS-runelf += runelf.c
$_DEPS-runelf += embox.fs.core

$_BRIEF-runelf = Execute elf file

define $_DETAILS-runelf
	NAME
		runelf - execute elf file
	SYNOPSIS
		runelf [-h] [-f filename]
	DESCRIPTION
		Execute elf file.
	OPTIONS
		-f filename - Elf file.
		-h          - show this help.
	EXAMPLES
		runelf -f image
	SEE ALSO
		readelf
	AUTHORS
		Avdyukhin Dmitry
endef
