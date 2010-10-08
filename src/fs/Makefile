
$_PACKAGE := embox.fs


$_MODS += rootfs file driver_allocator

$_SRCS-rootfs += rootfs.c
$_SRCS-file += file.c
$_SRCS-file += filesystems.c
$_SRCS-file += node.c
$_SRCS-driver_allocator += fs_driver.c
$_DEPS-rootfs += driver_allocator

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include
