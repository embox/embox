$_MODS += mac
$_SRCS-mac += mac.c
$_DEPS-mac += embox.driver.mac

$_BRIEF-mac = show physical address of the net device

define $_DETAILS-mac
	NAME
		mac - show physical address
	SYNOPSIS
		mac
	DESCRIPTION
		mac is a utility for displaying information
		of the net device.
	OPTIONS
		-f print full information
	AUTHOR
		Gleb Efimov
endef
