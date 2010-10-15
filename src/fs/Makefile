
$_PACKAGE := embox.fs


$_MODS += rootfs core driver_allocator node

$_SRCS-rootfs += rootfs.c
$_SRCS-rootfs += vfs.c
$_SRCS-core += file.c
$_SRCS-core += filesystems.c
$_SRCS-node += node.c
$_SRCS-driver_allocator += fs_driver.c
$_DEPS-core += driver_allocator node
$_DEPS-rootfs += core

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include

#describe special section
ifdef CONFIG_FS
$_LDFLAGS += $($_lds)

$_lds := $(OBJ_DIR)/fs/ramfs/fs.lds
$(IMAGE): $($_lds)

$($_lds): $($_SELFDIR)/fs.lds.S $(AUTOCONF_DIR)/config.lds.h
	mkdir -p $(@D) && $(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $($_lds).d
endif
