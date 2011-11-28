$_MODS += ls
$_SRCS-ls += ls.c
$_DEPS-ls += embox.fs.core

$_BRIEF-ls = List directory contents

define $_DETAILS-ls
	NAME
		ls - list directory contents
	SYNOPSIS
		ls [OPTION]... [FILE]...
	DESCRIPTION
		List information about the FILEs
		(the current directory by default).
	OPTIONS
		FILE -  path of directory.
			defaults '/' (root dir).
			Path format is /dir/filename.
		-l - use a long listing format
		-R - list subdirectories recursively
	AUTHORS
		Anton Bondarev
endef
