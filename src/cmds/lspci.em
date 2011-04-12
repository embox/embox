$_MODS += lspci
$_SRCS-lspci += lspci.c
$_DEPS-lspnp += embox.driver.pci

$_BRIEF-lspci = Show list pci's devices

define $_DETAILS-lspci
	NAME
		lspci - Print list devices on pci bus
	SYNOPSIS
		lspci
	AUTHOR
		Gleb Efimov
endef
