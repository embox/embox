$_PACKAGE := embox.kernel.thread

$_APIS += sched_api

$_MODS      += core sched sched_critical

$_SRCS-core += core.c
$_DEPS-core += sched
$_DEPS-core += sync
$_DEPS-core += embox.hal.context

$_SRCS-sched += sched.c
$_DEPS-REQUIRES += embox.kernel.timer.timer_api
$_DEPS-sched += embox.hal.context
#$_DEPS-sched += embox.kernel.critical.core
$_REQUIRES-sched += embox.kernel.thread.sched_policy.api
$_PROVIDES-sched += sched_api

$_SRCS-sched_critical += sched_critical.h
$_PROVIDES-sched_critical += sched_api
