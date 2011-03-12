
$_MODS += boot
$_SRCS-boot += boot.c
$_DEPS-boot += embox.fs.core
$_DEPS-boot += embox.hal.interrupt
$_DEPS-boot += embox.hal.prom
$_DEPS-boot += embox.hal.mm.mmu

$_BRIEF-boot = Boot application image from memory

define $_DETAILS-boot
	NAME
		boot - boot application image from memory
	SYNOPSIS
		boot [-f format] -a addr
	DESCRIPTION
		-f format - image format
			u - Das U-Boot format
			r - raw format (default)
		-a addr   - entry point address
	EXAMPLES
		For sparc:
		boot -a 0x40004000
		boot -f u -a 0x40003FC0
		For microblaze:
		boot -f u -a 0x45000000
	SEE ALSO
		tftp, goto
	AUTHORS
		Nikolay Korotky
endef
