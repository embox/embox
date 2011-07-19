
$_PACKAGE := core.kernel

$_CPPFLAGS += -D__KERNEL__
#TODO mb-gcc doesn't recognize multiple -T flag
#$_LDFLAGS  += -T $($_SELFDIR)/nosys.lds
$_LDFLAGS  += $($_SELFDIR)/nosys.lds

$(IMAGE): $($_SELFDIR)/nosys.lds

$_MODS_CORE += main
# TODO too more mods fro core.kernel
$_MODS += irq timer softirq drv_subsys #printk
$_MODS += printk
$_MODS += irq_critical softirq_critical

$_APIS += softirq_api irq_api

#$_SRCS-main += main.c
$_SRCS-main += init.c
$_SRCS-main += nosys.c
$_DEPS-main += embox.hal.arch
#$_DEPS-main += irq
$_DEPS-main += timer
#$_DEPS-main += printk
#$_DEPS-main += softirq
$_REQUIRES-main += embox.hal.diag
$_REQUIRES-main += softirq_api
$_REQUIRES-main += irq_api
$_REQUIRES-main += core.kernel.sched_api

ifdef CONFIG_THREAD
#$_DEPS-main += core.kernel.thread.core
endif

ifdef CONFIG_MMU_SUPPORT
$_DEPS-main += embox.hal.mm.mmu
endif

$_SRCS-softirq_critical += softirq_critical.h
$_PROVIDES-softirq_critical += softirq_api

$_SRCS-irq_critical += irq_critical.[c|h]
$_PROVIDES-irq_critical += irq_api

$_SRCS-irq     += irq.c irq_critical.[c|h]
$_REQUIRES-irq += embox.hal.interrupt
$_PROVIDES-irq += irq_api
# $_DEPS-irq     += softirq
# $_DEPS-irq     += core.kernel.critical.core

$_SRCS-softirq += softirq.c
$_PROVIDES-softirq += softirq_api
  #$_DEPS-irq     += core.kernel.critical.core

#ifdef CONFIG_PRINTK
#  $_SRCS-printk  += printk.c
#  $_SRCS-printk  += printk/putchar.c ../lib/stdio/printf.c
#  $_SRCS-printk  += printk/putchar.c printk/printf.c
#endif

ifdef CONFIG_TIMER
  $_SRCS-timer     += timer.c
  $_REQUIRES-timer += embox.hal.clock
endif
ifdef CONFIG_DRIVER_SUBSYSTEM
# It's core.kernel.drv_subsys
  $_MODS += drv_subsys
  $_SRCS-drv_subsys += driver.c
  $_DEPS-drv_subsys += embox.hal.mm.slob
endif

ifdef CONFIG_WATCHDOG
  $_DEPS-main += embox.driver.abstract.watchdog
endif

ifdef CONFIG_MEASURE
  $_MODS += measure
  $_SRCS-measure += measure.c
  $_DEPS-measure += $(CONFIG_MEASURE) timer
  $_DEPS-main += measure
endif
