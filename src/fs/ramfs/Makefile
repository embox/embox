
# Auto-generated package declaration inherited from parent directory.
# TODO Suggested renaming package to fs.ramfs
$_PACKAGE := embox.fs

$_MODS = ramfs

$_SRCS-ramfs += ramfs.c
$_DEPS-ramfs += core

#describe special section
ifdef CONFIG_RAMFS_CPIO

$_SRCS-ramfs += initramfs.c
$_SRCS-ramfs += ramfs_cpio.S
$_LDFLAGS += $($_lds)

$_lds := $(OBJ_DIR)/fs/ramfs/ramfs.lds
$(IMAGE): $($_lds)

$($_lds): $($_SELFDIR)/ramfs.lds.S $(AUTOCONF_DIR)/config.lds.h
	mkdir -p $(@D) && $(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $($_lds).d
endif
