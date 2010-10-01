
$_MODS_CORE += locore

$_SRCS-locore += head.S

$_SRCS-arch += arch.c

$_SRCS-interrupt += entry.S


DEFAULT_SYS_TIMER = embox.driver.timers.at91_timer
DEFAULT_SYS_IRQCTRL = embox.driver.interrupts.at91_interrupt
DEFAULT_SYS_WATCHDOG = embox.driver.watchdogs.at91_watchdog
DEFAULT_SYS_MOD_DIAG = embox.driver.serial.at91_uart

include $(MK_DIR)/system.mk

