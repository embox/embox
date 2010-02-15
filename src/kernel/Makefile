
$_PACKAGE := embox.kernel

$_MODS_ESSENTIAL += main
$_MODS += irq timer printk softirq

$_SRCS-main += main.c
$_SRCS-main += init.c
$_SRCS-main += panic.c
$_SRCS-irq += irq.c
$_SRCS-softirq += softirq.c
$_SRCS-printk += printk.c
$_SRCS-timer += timers_common.c

$_DEPS-main += printk
$_DEPS-main += embox.hal.arch
ifdef CONFIG_IRQ
  $_DEPS-main += irq
endif
ifdef CONFIG_TIMER
  $_DEPS-main += timer
endif

$_DEPS-irq += embox.hal.interrupt
ifdef CONFIG_SOFTIRQ
  $_DEPS-irq += softirq
endif

$_DEPS-timer += embox.hal.clock
$_DEPS-timer += embox.hal.clock
$_DEPS-printk += embox.hal.diag

$_CPPFLAGS += -D__KERNEL__
