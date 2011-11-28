
$_PACKAGE := embox.kernel

$_LDFLAGS  += $($_SELFDIR)/nosys.lds

$(IMAGE): $($_SELFDIR)/nosys.lds

$_CPPFLAGS += -I$(SRC_DIR)/include/posix

$_MODS_CORE += main

$_SRCS-main += init.c
$_SRCS-main += nosys.c

$_DEPS-main += embox.hal.arch

ifdef CONFIG_MMU_SUPPORT
$_DEPS-main += embox.mem.mmu
endif

