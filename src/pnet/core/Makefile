
$_PACKAGE := embox.pnet

$_APIS += rx_worker_api

$_MODS += core
$_SRCS-core += node.c proto.c pack.c connect.c core.c
$_DEPS-core += dev prior_path embox.pnet.node.null
$_REQUIRES-core += rx_worker_api

$_MODS += rx_thread
$_SRCS-rx_thread += rx_thread.c
$_PROVIDES-rx_thread += rx_worker_api

$_MODS += rx_simple
$_SRCS-rx_simple += rx_simple.c
$_PROVIDES-rx_simple += rx_worker_api

$_MODS += dev
$_SRCS-dev += dev.c
$_DEPS-dev += embox.pnet.node.null

$_MODS += prior_path
$_SRCS-prior_path += prior_path.c
