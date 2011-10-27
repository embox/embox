
$_PACKAGE := embox.pnet

$_MODS += socket
$_SRCS-socket += socket.c
$_DEPS-socket += embox.pnet.core

$_MODS 	              += pnet_entry
$_SRCS-pnet_entry     += pnet_entry.c
$_PROVIDES-pnet_entry += embox.net.api_entry

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include
$_CPPFLAGS += -I$(SRC_DIR)/include/posix

