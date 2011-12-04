
$_PACKAGE := embox.fs

$_MODS += rootfs core node file_desc


$_SRCS-core += file.c
$_SRCS-core += fs_drv.c
$_SRCS-core += vfs.c
$_SRCS-core += fs.c

$_SRCS-node += node.c
$_SRCS-file_desc += file_desc.c
$_DEPS-core += node file_desc

$_DEPS-rootfs += core

$_CPPFLAGS += -I$(SRC_DIR)/compat/linux/include
$_CPPFLAGS += -I$(SRC_DIR)/compat/posix/include
