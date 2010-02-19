
$_MODS_ESSENTIAL += locore

$_SRCS-locore += head.S entry.S trap_*.S sys.c
$_SRCS-arch += arch.c
$_SRCS-clock += clock_gptimer.c
$_SRCS-interrupt += interrupt_irqmp.c


$_DEPS-locore += interrupt
$_DEPS-clock += embox.kernel.irq
ifdef CONFIG_AMBAPP
  $_DEPS-clock += embox.drivers.ambapp
  $_DEPS-interrupt += embox.drivers.ambapp
endif
