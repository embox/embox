$_MODS += cat
$_SRCS-cat += cat.c
$_DEPS-cat += embox.fs.core

$_BRIEF-cat = Concatenate files and print on the standard output

define $_DETAILS-cat
	NAME
		cat - concatenate files and print on the standard output
	SYNOPSIS
		cat [FILES]
	DESCRIPTION
		Concatenate FILE(s) to standard output.
	AUTHORS
		Nikolay Korotky
endef
