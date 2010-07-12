
# core.kernel package is always available.
# There is no need to add it's mods to dependency list of your mods.
$_PACKAGE := core.kernel

$_CPPFLAGS += -D__KERNEL__
#TODO mb-gcc doesn't recognize multiple -T flag
#$_LDFLAGS  += -T $($_SELFDIR)/nosys.lds
$_LDFLAGS  += $($_SELFDIR)/nosys.lds

$(IMAGE): $($_SELFDIR)/nosys.lds

$_MODS_CORE += main

$_MODS += irq timer printk softirq thread scheduler job

$_SRCS-main += main.c
$_SRCS-main += init.c
$_SRCS-main += nosys.c
$_SRCS-job += job.c
$_DEPS-main += embox.hal.arch
$_DEPS-main += irq
$_DEPS-main += timer
$_DEPS-main += printk
$_DEPS-main += softirq

ifdef CONFIG_THREAD
$_DEPS-main += thread scheduler
$_DEPS-main += embox.hal.context
$_SRCS-thread += mutex.c
$_SRCS-thread += thread.c
$_SRCS-thread += thread_heap.c
$_SRCS-thread += convar.c
$_SRCS-scheduler += scheduler.c

$_SRCS-scheduler     += $(CONFIG_THREAD)
endif

ifdef CONFIG_MMU_SUPPORT
$_DEPS-main += embox.hal.mm.mmu
endif

ifdef CONFIG_IRQ
  $_SRCS-irq     += irq.c
  $_DEPS-irq     += embox.hal.interrupt
  $_DEPS-irq     += softirq
endif
ifdef CONFIG_PRINTK
  $_SRCS-printk  += printk.c
  $_DEPS-printk  += $(CONFIG_MOD_DIAG)
endif
ifdef CONFIG_SOFTIRQ
  $_SRCS-softirq += softirq.c
endif
ifdef CONFIG_TIMER
  $_SRCS-timer   += timer.c
  $_DEPS-timer   += embox.hal.clock
endif
ifdef CONFIG_DRIVER_SUBSYSTEM
# embox.kernel.drv_subsys
  $_MODS += drv_subsys
  $_SRCS-drv_subsys += driver.c
  $_DEPS-drv_subsys += embox.hal.mm.kmalloc
endif
