
$_MODS += mount
$_SRCS-mount += mount.c
$_DEPS-mount += embox.fs.ramfs
$_DEPS-mount += embox.fs.core

$_BRIEF-mount = Mount a filesystem

define $_DETAILS-mount
	NAME
		mount - mount a filesystem
	SYNOPSIS
		mount [-h] src dir
	DESCRIPTION
		src: cpio
		dir: /ramfs
	AUTHORS
		Nikolay Korotky
endef
