$_MODS += cp
$_SRCS-cp += cp.c
$_DEPS-cp += embox.fs.core

$_BRIEF-cp = Copy file

define $_DETAILS-cp
	NAME
		cp - copy files and directories
	SYNOPSIS
		cp [OPTIONS] SOURCE DEST
	DESCRIPTION
		Copy SOURCE to DEST
	AUTHOR
		Roman Evstifeev
endef
