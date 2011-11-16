$_MODS += mmu_probe
$_SRCS-mmu_probe += mmu_probe.c
$_DEPS-mmu_probe += embox.hal.mm.mmu

#$_CPPFLAGS += -imacros $(AUTOCONF_DIR)/config.lds.h

$_BRIEF-mmu_probe = Testing mmu module

define $_DETAILS-mmu_probe
	NAME
		mmu_probe - testing mmu module.
	SYNOPSIS
		mmu_probe [-h] [-r]
	OPTIONS
		-h  - to see this page
		-r  - show mmu regs
	AUTHORS
		Anton Bondarev
endef
