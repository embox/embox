$_PACKAGE := embox.driver

$_MODS += tty
$_SRCS-tty += tty.c
$_DEPS-tty += embox.fs.devfs
ifdef
$_DEPS-tty += embox.driver.terminal #vtparse + vtbuilder
endif

$_MODS += vconsole
$_SRCS-vconsole += vconsole.c
$_DEPS-vconsole += tty
