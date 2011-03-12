$_MODS += mknod
$_SRCS-mknod += mknod.c
$_DEPS-mknod += embox.fs.core

$_BRIEF-mknod = Create special file

define $_DETAILS-mknod
	NAME
		mknode - create special file
	SYNOPSIS
		mknod NAME
	DESCRIPTION
		create virtual node in file system
	AUTHORS
		Anton Bondarev
endef
