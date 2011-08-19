####################################################
# Posix TTY Device Module
#---------------------------------------------------
#  - base ( required for all below )
####################################################
$_PACKAGE := embox.driver

#################################################################
# Virtual service, consists of screen,keyboard,console
# ---------------------------------------------------------------
$_APIS += embox.hal.ttyx

# TTY Factory Base
############################
$_MODS += tty_factory
#---------------------------
$_PROVIDES-tty_factory += embox.hal.ttyx
$_DEPS-tty_factory += core.kernel.timer.sleep_nosched
$_DEPS-tty_factory += core.kernel.timer.timer
$_DEPS-tty_factory += core.kernel.softirq
$_DEPS-tty_factory += core.kernel.irq
$_DEPS-tty_factory += embox.fs.devfs
$_SRCS-tty_factory += tty.c

# keyboard Depends on TTY base
################################
$_MODS += tty_kbd
#-------------------------------
$_REQUIRES-tty_kbd += embox.hal.ttyx
$_SRCS-tty_kbd += kbd.c
$_DEPS-tty_kbd += tty_factory

# screen Depends on TTY base
################################
#$_MODS += ttyo
#-------------------------------
#$_REQUIRES-ttyo += embox.hal.ttyx
#$_SRCS-ttyo += ttyo.c
#$_DEPS-ttyo += tty_factory

# RS232 Depends on TTY base
###############################
#$_MODS += ttys
#------------------------------
#$_REQUIRES-ttys += embox.hal.ttyx
#$_SRCS-ttys += ttys.c
#$_DEPS-ttys += tty_factory

# pseudo Depends on tty base
###############################
#$_MODS += ttyp
#------------------------------
#$_REQUIRES-ttyp += embox.hal.ttyx
#$_SRCS-ttyp += ttyp.c
#$_DEPS-ttyp += tty_factory
