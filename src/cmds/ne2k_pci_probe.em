$_MODS += ne2k_pci_probe
$_SRCS-ne2k_pci_probe += ne2k_pci_probe.c
$_DEPS-ne2k_pci_probe += embox.driver.net.ne2k_pci

$_BRIEF-ne2k_pci_probe = show information about the net device

define $_DETAILS-ne2k_pci_probe
	NAME
		ne2k_pci_probe - show information about the device
	SYNOPSIS
		ne2k_pci_probe
	DESCRIPTION
		ne2k_pci_probe is a utility for displaying information
		of the net device.
	OPTIONS
		-p show the net devices pages
		-f print full information
	AUTHOR
		Gleb Efimov, Ilia Vaprol
endef
