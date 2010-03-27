
$_MODS_CORE += locore

$_SRCS-locore += head.S
$_SRCS-locore += sys.c
$_SRCS-locore += traps.c

# TODO A temporal solution for testing unstable locore branch. -- Eldar
ifndef CONFIG_LOCORE_OBSOLETE
$_SRCS-locore += entry.S trap_*.S
else
$_SRCS-locore += locore_obsolete.S traps_obsolete.c
endif

$_SRCS-arch      += arch.c
$_SRCS-clock     += clock_gptimer.c
$_SRCS-interrupt += interrupt_irqmp.c
$_SRCS-traps 	 += traps_core.c

ifdef CONFIG_AMBAPP
  $_DEPS-clock     += embox.driver.ambapp
  $_DEPS-interrupt += embox.driver.ambapp
endif
