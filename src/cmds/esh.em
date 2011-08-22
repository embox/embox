$_MODS += esh
$_SRCS-esh += esh/esh.c
$_DEPS-esh += embox.kernel.thread.core
$_USES-esh += libframework.a

$_BRIEF-esh = Embox shell

define $_DETAILS-cp
	NAME
		esh - Embox shell
	SYNOPSIS

	DESCRIPTION

	AUTHOR
		Fedor Burdun
endef
