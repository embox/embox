
$_MODS_CORE += locore

$_SRCS-locore += head.S

$_SRCS-arch += arch.c

$_SRCS-interrupt += entry.S

DEFAULT_SYS_MOD_DIAG = embox.driver.serial.at91_uart

include $(MK_DIR)/system.mk

DEFAULT_SYS_WATCHDOG = embox.driver.watchdogs.at91_watchdog

ifdef CONFIG_SYS_WATCHDOG
$_DEPS-embox.driver.abstract.watchdog     += $(CONFIG_SYS_WATCHDOG)
else
#default watchdog
$_DEPS-embox.driver.abstract.watchdog     += $(DEFAULT_SYS_WATCHDOG)
endif
