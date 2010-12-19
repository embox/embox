$_PACKAGE := core.kernel.threads

$_MODS += core thread

ifdef CONFIG_THREAD

$_DEPS-core += scheduler synchronization
$_DEPS-core += embox.hal.context

ifdef CONFIG_PP_ENABLE
$_DEPS-thread += core.kernel.pp
endif

$_SRCS-thread += thread.c
$_SRCS-thread += thread_heap.c
endif
