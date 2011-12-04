
$_PACKAGE := embox.kernel

$_LDFLAGS  += $($_SELFDIR)/nosys.lds

$(IMAGE): $($_SELFDIR)/nosys.lds

$_CPPFLAGS += -I$(SRC_DIR)/compat/posix/include

$_MODS += main

$_SRCS-main += init.c
$_SRCS-main += nosys.c

$_DEPS-main += embox.hal.arch embox.hal.locore

ifdef CONFIG_MMU_SUPPORT
$_DEPS-main += embox.mem.mmu
endif

