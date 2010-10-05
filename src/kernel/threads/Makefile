ifdef CONFIG_THREAD
$_PACKAGE := core.kernel.threads

$_MODS += core thread

$_DEPS-core += scheduler synchronization
$_DEPS-core += embox.hal.context

$_SRCS-thread += thread.c
$_SRCS-thread += thread_heap.c
endif
