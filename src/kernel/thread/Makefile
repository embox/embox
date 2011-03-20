$_PACKAGE := core.kernel.thread

$_MODS += core thread

$_DEPS-core += core.kernel.thread.sched.core
$_DEPS-core += thread
$_DEPS-core += sync
$_DEPS-core += embox.hal.context
ifdef CONFIG_PP_ENABLE
$_DEPS-core += core.kernel.pp
endif

$_SRCS-thread += thread.c
$_SRCS-thread += thread_heap.c
