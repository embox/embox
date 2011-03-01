ifdef CONFIG_THREAD
$_PACKAGE := core.kernel.threads
$_MODS += scheduler

$_DEPS-scheduler += thread core.kernel.timer
$_SRCS-scheduler += scheduler.c
$_SRCS-scheduler += $(CONFIG_THREAD)
endif
