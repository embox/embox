$_PACKAGE := embox.driver

$_MODS += tty
$_SRCS-tty += tty_with_consoles/tty.c tty_with_consoles/tty_action.c
$_DEPS-tty += embox.fs.devfs
$_USES-tty += libc.a
#to-do: remove this?
#need for vtbuilder, vtparser
$_DEPS-tty += embox.driver.terminal
$_DEPS-tty += embox.cmd.esh
$_DEPS-tty += core.kernel.thread.sched
$_DEPS-tty += tty_common
$_REQUIRES-tty += embox.hal.diag

$_MODS += tty_one_console
$_SRCS-tty_one_console += tty_one_console/tty_one_console.c tty_one_console/tty_action_oc.c
$_DEPS-tty_one_console += embox.fs.devfs
$_USES-tty_one_console += libc.a
#to-do: remove this?
#need for vtbuilder, vtparser
$_DEPS-tty_one_console += embox.driver.terminal
$_DEPS-tty_one_console += embox.cmd.esh
$_DEPS-tty_one_console += tty_common
$_REQUIRES-tty_one_console += embox.hal.diag

$_MODS += tty_common
$_SRCS-tty_common += tty_common/tty_action_common.c tty_common/vconsole.c
