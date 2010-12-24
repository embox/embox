$_PACKAGE := embox.driver

$_MODS += tty
$_SRCS-tty += tty.c
$_DEPS-tty += embox.fs.devfs tty_action

#it isn't good idea
$_DEPS-tty += embox.driver.serial.apbuart
#$_DEPS-tty += embox.driver.ambapp
ifdef
$_DEPS-tty += embox.driver.terminal #vtparse + vtbuilder
endif

ifdef CONFIG_TTY_CONSOLE_COUNT
$_DEPS-tty += core.kernel.pp
$_DEPS-tty += embox.cmd.esh
$_DEPS-tty += core.kernel.threads.scheduler
endif

$_MODS += vconsole
$_SRCS-vconsole += vconsole.c
$_DEPS-vconsole += tty tty_action

$_MODS += tty_action
$_SRCS-tty_action += tty_action.c
#$_DEPS-vtaction
