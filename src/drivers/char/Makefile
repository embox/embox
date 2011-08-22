
# Auto-generated package declaration inherited from parent directory.
# TODO Suggested renaming package to drivers.char
$_PACKAGE := embox.driver


$_MODS += terminal

$_SRCS-terminal += vt*.c
$_SRCS-terminal += terminal.c


ifdef CONFIG_DRIVER_SUBSYSTEM
$_MODS += zero
$_SRCS-zero += zero.c
$_DEPS-zero += embox.kernel.drv_subsys

$_MODS += pipe
$_SRCS-pipe += pipe.c
$_DEPS-pipe += embox.kernel.drv_subsys
endif
