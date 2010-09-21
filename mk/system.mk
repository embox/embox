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
