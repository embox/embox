
$_MODS_CORE += locore

# XXX agrhh... -- Eldar
#$_MODS_CORE += context

$_SRCS-locore += head.S
$_SRCS-locore += sys.c
$_SRCS-locore += traps.c

# TODO A temporal solution for testing unstable locore branch. -- Eldar
ifndef CONFIG_LOCORE_OBSOLETE
$_SRCS-locore += entry.S trap_*.S winflush.S
else
$_SRCS-locore += locore_obsolete.S traps_obsolete.c
endif

$_SRCS-arch      += arch.c
$_SRCS-context   += context.c context_switch.S

ifdef CONFIG_SYS_TIMER
$_SRCS-clock     += $(CONFIG_SYS_TIMER)
else
$_SRCS-clock     += clock_gptimer.c
endif

ifdef CONFIG_SYS_INTCRL_DEFINED
$_DEPS-interrupt += $(CONFIG_SYS_INTCRL_DEFINED)
else
$_SRCS-interrupt += interrupt_irqmp.c
endif

$_SRCS-traps 	 += traps_core.c

ifdef CONFIG_AMBAPP
  $_DEPS-clock     += embox.driver.ambapp
  $_DEPS-interrupt += embox.driver.ambapp
endif

$_SRCS-prom += prom.c

