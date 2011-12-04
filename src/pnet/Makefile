
$_PACKAGE := embox.pnet

$_APIS += entry_api

$_MODS += socket
$_SRCS-socket += socket.c
$_DEPS-socket += embox.pnet.core

$_MODS 	              += pnet_entry
$_SRCS-pnet_entry     += pnet_entry.c
$_DEPS-pnet_entry     += embox.pnet.core
$_PROVIDES-pnet_entry += embox.pnet.entry_api

$_MODS               += net_entry
$_SRCS-net_entry     += net_entry.c
$_DEPS-net_entry     += embox.pnet.core embox.pnet.core.pack.pack_skbuf
$_PROVIDES-net_entry += embox.pnet.entry_api embox.net.entry_api


$_CPPFLAGS += -I$(SRC_DIR)/compat/linux/include
$_CPPFLAGS += -I$(SRC_DIR)/compat/posix/include

