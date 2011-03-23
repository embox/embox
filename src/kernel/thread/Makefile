$_PACKAGE := core.kernel.thread

$_MODS      += core
$_SRCS-core += core.c
$_DEPS-core += core.kernel.thread.sched
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
$_DEPS-sched += core.kernel.timer
$_DEPS-sched += embox.hal.context

