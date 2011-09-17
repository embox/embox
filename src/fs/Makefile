
$_PACKAGE := embox.fs

$_MODS += rootfs core driver_allocator node file_desc


$_CPPFLAGS += -I$(SRC_DIR)/include/posix

$_SRCS-core += file.c
$_SRCS-core += fs_drv.c
$_SRCS-core += vfs.c
$_SRCS-core += fs.c
$_SRCS-node += node.c
$_SRCS-file_desc += file_desc.c
$_DEPS-core += node file_desc

$_DEPS-rootfs += core

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include
$_CPPFLAGS += -I$(SRC_DIR)/include/posix
