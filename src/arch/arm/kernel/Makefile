
$_MODS_CORE += locore

$_SRCS-locore += head.S

$_SRCS-arch += arch.c

$_SRCS-interrupt += entry.S


DEFAULT_SYS_TIMER = embox.driver.timers.at91_timer
DEFAULT_SYS_IRQCTRL = embox.driver.interrupts.at91_interrupt

include $(MK_DIR)/system.mk

