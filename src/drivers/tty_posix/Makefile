####################################################
# Posix TTY Device Module
####################################################
$_PACKAGE := embox.driver

$_MODS += tty_posix

$_DEPS-tty_posix += core.kernel.timer.sleep_nosched
$_DEPS-tty_posix += core.kernel.timer.timer
$_DEPS-tty_posix += core.kernel.softirq
$_DEPS-tty_posix += core.kernel.irq
$_DEPS-tty_posix += embox.fs.devfs

$_SRCS-tty_posix += keyboard.c
#$_SRCS-tty_posix += console.c
#$_SRCS-tty_posix += pty.c
#$_SRCS-tty_posix += rs232.c
$_SRCS-tty_posix += tty.c

$_BRIEF-tty_posix = TTY posix implementation
