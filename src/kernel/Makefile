
# core.kernel package is always available.
# There is no need to add it's mods to dependency list of your mods.
$_PACKAGE := core.kernel

$_CPPFLAGS += -D__KERNEL__
#TODO mb-gcc doesn't recognize multiple -T flag
#$_LDFLAGS  += -T $($_SELFDIR)/nosys.lds
$_LDFLAGS  += $($_SELFDIR)/nosys.lds

$(IMAGE): $($_SELFDIR)/nosys.lds

$_MODS_CORE += main
# TODO too more mods fro core.kernel
$_MODS += irq timer printk softirq job elf drv_subsys

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
$_DEPS-main += core.kernel.threads.core
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
  #$_SRCS-printk  += printk/putchar.c ../lib/stdio/printf.c
  $_SRCS-printk  += printk/putchar.c printk/printf.c
  $_DEPS-printk  += embox.driver.abstract.mod_diag
endif
ifdef CONFIG_SOFTIRQ
  $_SRCS-softirq += softirq.c
endif
ifdef CONFIG_TIMER
  $_SRCS-timer   += timer.c
  $_DEPS-timer   += embox.hal.clock
endif
ifdef CONFIG_ELF
  $_DEPS-main += elf
  $_SRCS-elf += elf_executer.c
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
