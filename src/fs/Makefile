
$_PACKAGE := embox.fs

$_MODS += rootfs core driver_allocator node

$_SRCS-core += file.c
$_SRCS-core += filesystems.c
$_SRCS-core += vfs.c
$_SRCS-node += node.c
$_SRCS-driver_allocator += fs_driver.c
$_DEPS-core += driver_allocator node
#FIXME: core doesn't depend on rootfs actually.
#$_DEPS-core += rootfs
$_DEPS-rootfs += core

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include
