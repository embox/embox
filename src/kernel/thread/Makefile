$_PACKAGE := core.kernel.thread

$_MODS      += core
$_SRCS-core += core.c
$_DEPS-core += sched
$_DEPS-core += state
$_DEPS-core += sync
$_DEPS-core += embox.hal.context
ifdef CONFIG_PP_ENABLE
$_DEPS-core += core.kernel.pp
endif

$_MODS       += sched
$_SRCS-sched += sched.c
ifdef CONFIG_SCHED_STRATEGY
$_DEPS-sched += core.kernel.thread.sched_policy.$(CONFIG_SCHED_STRATEGY)
endif
$_DEPS-sched += state
$_DEPS-sched += core.kernel.timer
$_DEPS-sched += embox.hal.context
$_DEPS-sched += core.kernel.critical.core

$_MODS       += state
$_SRCS-state += state.c
