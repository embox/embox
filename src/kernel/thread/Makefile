$_PACKAGE := core.kernel.thread

$_APIS += sched_api

$_MODS      += core sched nosched

$_SRCS-core += core.c
$_DEPS-core += sched
$_DEPS-core += sync
$_DEPS-core += embox.hal.context
$_REQUIRES-core += core.kernel.softirq_api
$_REQUIRES-core += core.kernel.irq_api
$_REQUIRES-core += sched_api

$_SRCS-sched += sched.c
$_DEPS-sched += core.kernel.timer
$_DEPS-sched += embox.hal.context
$_DEPS-sched += core.kernel.critical.core
$_REQUIRES-sched += core.kernel.thread.sched_policy.api
$_PROVIDES-sched += sched_api

$_SRCS-nosched += nosched.c
$_PROVIDES-nosched += sched_api
