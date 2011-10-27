
$_PACKAGE := embox.pnet

$_MODS += core
$_SRCS-core += node.c proto.c pack.c connect.c core.c
$_DEPS-core += dev prior_path rx_thread

$_MODS += rx_thread
$_SRCS-rx_thread += rx_thread.c

$_MODS += dev
$_SRCS-dev += dev.c
$_DEPS-dev += embox.pnet.node.null

$_MODS += prior_path
$_SRCS-prior_path += prior_path.c
