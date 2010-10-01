ifdef CONFIG_SYS_TIMER
$_DEPS-clock     += $(CONFIG_SYS_TIMER)
else
#default timer
$_DEPS-clock     += $(DEFAULT_SYS_TIMER)
endif

ifdef CONFIG_SYS_IRQCTRL
$_DEPS-interrupt += $(CONFIG_SYS_IRQCTRL)
else
#default interrupt controller
$_DEPS-interrupt += $(DEFAULT_SYS_IRQCTRL)
endif

ifdef CONFIG_SYS_WATCHDOG
$_DEPS-embox.driver.abstract.watchdog     += $(CONFIG_SYS_WATCHDOG)
else
#default watchdog
$_DEPS-embox.driver.abstract.watchdog     += $(DEFAULT_SYS_WATCHDOG)
endif

ifdef CONFIG_SYS_MOD_DIAG
$_DEPS-embox.driver.abstract.mod_diag     += $(CONFIG_SYS_MOD_DIAG)
else
#default timer
$_DEPS-embox.driver.abstract.mod_diag     += $(DEFAULT_SYS_DIAG)
endif
