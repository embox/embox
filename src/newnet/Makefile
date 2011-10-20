
$_PACKAGE := embox.newnet

$_MODS += core
$_SRCS-core += core/node.c core/proto.c core/core.c
$_DEPS-core += dev socket

$_MODS += socket
$_SRCS-socket += core/socket.c

$_MODS += dev
$_SRCS-dev += core/dev.c

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include
$_CPPFLAGS += -I$(SRC_DIR)/include/posix
