
$_PACKAGE := embox.pnet

$_CPPFLAGS += -I$(SRC_DIR)/linux-compat/include
$_CPPFLAGS += -I$(SRC_DIR)/include/posix

$_MODS += core
$_SRCS-core += node.c proto.c core.c
$_DEPS-core += dev socket prior_path rx_thread

$_MODS += rx_thread
$_SRCS-rx_thread += rx_thread.c

$_MODS += socket
$_SRCS-socket += socket.c

$_MODS += dev
$_SRCS-dev += dev.c

$_MODS += prior_path
$_SRCS-prior_path += prior_path.c
