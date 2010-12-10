$_PACKAGE := embox.driver

$_MODS += tty
$_SRCS-tty += tty.c
$_DEPS-tty += embox.fs.devfs tty_action
ifdef
$_DEPS-tty += embox.driver.terminal #vtparse + vtbuilder
endif

$_MODS += vconsole
$_SRCS-vconsole += vconsole.c
$_DEPS-vconsole += tty tty_action

$_MODS += tty_action
$_SRCS-tty_action += tty_action.c
#$_DEPS-vtaction
