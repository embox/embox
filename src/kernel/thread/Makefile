$_PACKAGE := core.kernel.thread

$_MODS      += core
$_SRCS-core += core.c
$_DEPS-core += sched
$_DEPS-core += sync
$_DEPS-core += embox.hal.context

$_MODS       += sched
$_SRCS-sched += sched.c
$_DEPS-sched += core.kernel.timer
$_DEPS-sched += embox.hal.context
$_DEPS-sched += core.kernel.critical.core
$_REQUIRES-sched += core.kernel.thread.sched_policy.api

$_MODS       += nosched
$_SRCS-nosched += nosched.c
$_DEPS-nosched += core.kernel.critical.core
