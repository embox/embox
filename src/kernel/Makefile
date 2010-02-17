
config = $(if $(CONFIG_$1),,nosys/)

$_SUBDIRS :=

$_PACKAGE := embox.kernel

$_CPPFLAGS += -D__KERNEL__

$_MODS_ESSENTIAL += main
$_MODS += irq timer printk softirq diag

$_SRCS-main    += main.c
$_SRCS-main    += init.c
$_SRCS-main    += panic.c
$_SRCS-printk  += printk.c
$_SRCS-irq     += $(call config,IRQ)irq.c
$_SRCS-softirq += $(call config,SOFTIRQ)softirq.c
$_SRCS-timer   += $(call config,TIMER)timer.c

$_DEPS-main    += printk
$_DEPS-main    += irq
$_DEPS-main    += timer
$_DEPS-main    += embox.hal.arch
$_DEPS-irq     += embox.hal.interrupt
$_DEPS-timer   += embox.hal.clock
$_DEPS-printk  += diag

ifdef CONFIG_IRQ
$_DEPS-irq     += softirq
endif

ifdef CONFIG_MOD_DIAG
  $_DEPS-diag += $(CONFIG_MOD_DIAG)
else
  $_SRCS-diag += nosys/diag.c
endif
