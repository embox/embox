
$_MODS += terminal

$_SRCS-terminal += vt*.c
$_SRCS-terminal += terminal.c


ifdef CONFIG_DRIVER_SUBSYSTEM
$_MODS += zero
$_SRCS-zero += zero.c
$_DEPS-zero += core.kernel.drv_subsys

$_MODS += pipe
$_SRCS-pipe += pipe.c
$_DEPS-pipe += core.kernel.drv_subsys
endif
