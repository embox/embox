
$_MODS_CORE += locore

$_SRCS-locore += head.S
$_SRCS-locore += sys.c
$_SRCS-locore += traps.c

$_SRCS-locore += entry.S trap_*.S winflush.S

$_SRCS-arch      += arch.c
$_SRCS-context   += context.c context_switch.S

DEFAULT_SYS_TIMER = embox.driver.timers.gptimer
DEFAULT_SYS_MOD_DIAG = embox.driver.serial.diag_apbuart

include $(MK_DIR)/system.mk

$_SRCS-prom += prom.c

