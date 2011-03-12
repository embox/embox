
$_MODS += load
$_SRCS-load += load.c
$_DEPS-load += embox.fs.core
$_DEPS-load += embox.fs.ramfs

$_BRIEF-load = Load image file into memory

define $_DETAILS-load
	NAME
		load - map file into mem segment
	DESCRIPTION
		copy text image from ROM to RAM
	SYNOPSIS
		load [-a addr] [-f filename] [-h]
	SEE ALSO
		goto, boot, runelf, mount
	AUTHORS
		Sergey Kuzmin
endef
