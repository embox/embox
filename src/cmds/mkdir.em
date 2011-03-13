$_MODS += mkdir
$_SRCS-mkdir += mkdir.c
$_DEPS-mkdir += embox.fs.core

$_BRIEF-mkdir = Make directories

define $_DETAILS-mkdir
	NAME
	    	mkdir - make directories
	SYNOPSIS
		mkdir DIRECTORY ...
	DESCRIPTION
		Create the DIRECTORY(ies), if they do not already exist.
	AUTHORS
		Nikolay Korotky
endef
