$_PACKAGE := core.kernel.thread

$_MODS      += core
$_SRCS-core += core.c
$_DEPS-core += sched
$_DEPS-core += sync
$_DEPS-core += embox.hal.context

$_MODS       += sched
$_SRCS-sched += sched.c
$_DEPS-REQUIRES += core.kernel.timer.timer_api
$_DEPS-sched += embox.hal.context
$_DEPS-sched += core.kernel.critical.core
$_REQUIRES-sched += core.kernel.thread.sched_policy.api
