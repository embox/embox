$_MODS += meminfo
$_SRCS-meminfo += meminfo.c
$_DEPS-meminfo += embox.hal.mm.slab

$_BRIEF-meminfo = Write memory statistic for kmalloc or mpallocator

define $_DETAILS-meminfo
	NAME
		meminfo - shows memory statistic for kmalloc, mpallocator, slab allocator
	SYNOPSIS
		meminfo [-h] [-k] [-m] -s
	OPTIONS
		-h	- to see this page
		-m	- to see memory statistic for mpallocator
		-k	- to see memory statistic for kmalloc
		-s	- to see memory statistic for slab allocator
	AUTHORS
		Kirill Tyushev, Alexandr Kalmuk, Dmitry Zubarevich
endef
