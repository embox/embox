
$_MODS += slab

$_SRCS-libutil.a += pool.c

ifdef CONFIG_SLAB
$_SRCS-slab += $(CONFIG_SLAB)
endif
$_DEPS-slab += embox.hal.mm.mpallocator

