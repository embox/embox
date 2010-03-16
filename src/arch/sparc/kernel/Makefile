
$_MODS_CORE += locore

$_SRCS-locore += head.S
$_SRCS-locore += sys.c

LOCORE_OBSOLETE = 1
ifndef LOCORE_OBSOLETE
$_SRCS-locore += entry.S trap_*.S
else
$_SRCS-locore += locore_obsolete.S traps_obsolete.c
endif

$_SRCS-arch += arch.c
$_SRCS-clock += clock_gptimer.c
$_SRCS-interrupt += interrupt_irqmp.c

ifdef CONFIG_AMBAPP
  $_DEPS-clock += embox.driver.ambapp
  $_DEPS-interrupt += embox.driver.ambapp
endif
