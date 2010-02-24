
$_MODS_CORE += locore

$_SRCS-locore += head.S entry.S trap_*.S sys.c
$_SRCS-arch += arch.c
$_SRCS-clock += clock_gptimer.c
$_SRCS-interrupt += interrupt_irqmp.c

ifdef CONFIG_AMBAPP
  $_DEPS-clock += embox.driver.ambapp
  $_DEPS-interrupt += embox.driver.ambapp
endif
