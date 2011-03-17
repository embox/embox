
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

#$_SRCS-main += main.c
$_SRCS-main += init.c
$_SRCS-main += nosys.c
$_DEPS-main += embox.hal.arch
$_DEPS-main += irq
$_DEPS-main += timer
#$_DEPS-main += printk
$_DEPS-main += softirq
$_REQUIRES-main += embox.hal.diag

ifdef CONFIG_THREAD
$_DEPS-main += core.kernel.thread.core
endif

ifdef CONFIG_MMU_SUPPORT
$_DEPS-main += embox.hal.mm.mmu
endif

ifdef CONFIG_IRQ
  $_SRCS-irq     += irq.c
  $_REQUIRES-irq += embox.hal.interrupt
  $_DEPS-irq     += softirq
endif
#ifdef CONFIG_PRINTK
 # $_SRCS-printk  += printk.c
  #$_SRCS-printk  += printk/putchar.c ../lib/stdio/printf.c
  #$_SRCS-printk  += printk/putchar.c printk/printf.c
 # $_DEPS-printk  += embox.driver.abstract.mod_diag
#endif
ifdef CONFIG_SOFTIRQ
  $_SRCS-softirq += softirq.c
endif
ifdef CONFIG_TIMER
  $_SRCS-timer     += timer.c
  $_REQUIRES-timer += embox.hal.clock
endif
ifdef CONFIG_DRIVER_SUBSYSTEM
# It's core.kernel.drv_subsys
  $_MODS += drv_subsys
  $_SRCS-drv_subsys += driver.c
  $_DEPS-drv_subsys += embox.hal.mm.kmalloc
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
