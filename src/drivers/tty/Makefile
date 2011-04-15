$_PACKAGE := embox.driver

$_MODS += tty
$_SRCS-tty += tty.c
$_DEPS-tty += embox.fs.devfs tty_action
$_USES-tty += libc.a

$_REQUIRES-tty += embox.hal.diag

ifdef CONFIG_TTY_CONSOLE_COUNT
$_DEPS-tty += embox.cmd.esh
$_DEPS-tty += core.kernel.thread.sched
endif

$_MODS += vconsole
$_SRCS-vconsole += vconsole.c
$_DEPS-vconsole += tty tty_action

$_MODS += tty_action
$_SRCS-tty_action += tty_action.c
