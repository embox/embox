
$_PACKAGE := embox.kernel

#TODO mb-gcc doesn't recognize multiple -T flag
#$_LDFLAGS  += -T $($_SELFDIR)/nosys.lds
$_LDFLAGS  += $($_SELFDIR)/nosys.lds

$(IMAGE): $($_SELFDIR)/nosys.lds

$_MODS_CORE += main
# TODO too more mods fro embox.kernel
$_MODS += irq softirq drv_subsys #printk
$_MODS += printk
$_MODS += irq_critical softirq_critical irq_nested

$_APIS += softirq_api irq_api

#$_SRCS-main += main.c
$_SRCS-main += init.c
$_SRCS-main += nosys.c
$_DEPS-main += embox.hal.arch
#$_DEPS-main += irq
#$_DEPS-main += timer
$_DEPS-REQUIRES += embox.kernel.timer.timer_api
#$_DEPS-main += printk
#$_DEPS-main += softirq
#$_REQUIRES-main += embox.hal.diag
$_REQUIRES-main += softirq_api
$_REQUIRES-main += irq_api
$_REQUIRES-main += embox.kernel.sched_api

ifdef CONFIG_THREAD
#$_DEPS-main += embox.kernel.thread.core
endif

ifdef CONFIG_MMU_SUPPORT
$_DEPS-main += embox.hal.mm.mmu
endif

$_SRCS-irq_nested += irq_critical_nested.h

$_SRCS-softirq_critical += softirq_critical.h
$_PROVIDES-softirq_critical += softirq_api

$_SRCS-irq_critical += irq_critical.[c|h]
$_PROVIDES-irq_critical += irq_api

$_SRCS-irq     += irq.c irq_critical.[c|h]
$_DEPS-irq     += irq_nested
$_REQUIRES-irq += embox.hal.interrupt
$_PROVIDES-irq += irq_api
# $_DEPS-irq     += softirq
# $_DEPS-irq     += embox.kernel.critical.core

$_SRCS-softirq += softirq.c
$_PROVIDES-softirq += softirq_api
$_REQUIRES-softirq += embox.kernel.timer.timer_api
  #$_DEPS-irq     += embox.kernel.critical.core

#ifdef CONFIG_PRINTK
#  $_SRCS-printk  += printk.c
#  $_SRCS-printk  += printk/putchar.c ../lib/stdio/printf.c
#  $_SRCS-printk  += printk/putchar.c printk/printf.c
#endif

#ifdef CONFIG_TIMER
#  $_SRCS-timer     += timer.c
#  $_REQUIRES-timer += embox.hal.clock
#endif
ifdef CONFIG_DRIVER_SUBSYSTEM
# It's embox.kernel.drv_subsys
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

$_MODS                  += clock_source
$_SRCS-clock_source     += clock_source.c

$_MODS      += time
$_SRCS-time += time.c
$_REQUIRES-time += embox.kernel.timer.timer_api
$_DEPS-time += embox.kernel.clock_source



#########



SELFDIR ?= .
__CMD_EM_FILES := $(wildcard $(SELFDIR)/*.em)

include $(__CMD_EM_FILES)
