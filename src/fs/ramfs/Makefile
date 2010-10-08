
$_MODS = ramfs

$_SRCS-ramfs += ramfs.c
$_SRCS-ramfs += ramfs_cpio.S

#describe special section
ifdef CONFIG_RAMFS
$_LDFLAGS += $($_lds)

$_lds := $(OBJ_DIR)/fs/ramfs/ramfs.lds
$(IMAGE): $($_lds)

$($_lds): $($_SELFDIR)/ramfs.lds.S $(AUTOCONF_DIR)/config.lds.h
	mkdir -p $(@D) && $(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $($_lds).d
endif

