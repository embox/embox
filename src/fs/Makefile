
$_PACKAGE := embox.fs

$_MODS += ramfs rootfs file

$_SRCS-ramfs += ramfs.c
$_SRCS-ramfs += ramfs_cpio.S
$_SRCS-rootfs += rootfs.c
$_SRCS-file += file.c

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include

#describe spatial section
ifdef PIGGY
$_LDFLAGS += $($_piggy_lds)

$_piggy_lds := $(OBJ_DIR)/fs/piggy.lds
$(IMAGE): $($_piggy_lds)

$($_heap_lds): $($_SELFDIR)/piggy.lds.S $(AUTOCONF_DIR)/config.lds.h
	mkdir -p $(@D) && $(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $($_piggy_lds).d
endif
