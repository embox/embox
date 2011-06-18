$_MODS += lspci
$_SRCS-lspci += lspci.c
$_DEPS-lspci += embox.driver.pci

$_BRIEF-lspci = Show list pci's devices and information

define $_DETAILS-lspci
	NAME
		lspci - list all PCI devices
	SYNOPSIS
		lspci [hf]
	DESCRIPTION
		lspci is a utility for displaying information
		about PCI buses in the system and devices connected to them.
	OPTIONS
		-f print full information
	AUTHOR
		Gleb Efimov
endef
