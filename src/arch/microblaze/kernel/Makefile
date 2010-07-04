
$_MODS_CORE += locore context

#FIXME open_prom module must be specify
$_SRCS-prom + = prom.c

$_SRCS-locore += interrupt_handler.c sys.c entry.S boot.S mb_trap_dispatcher.c

$_SRCS-arch += arch.c
$_SRCS-context += context.c
$_SRCS-traps += traps_core.c


ifdef CONFIG_SYS_TIMER_DEFINED
$_DEPS-clock += $(CONFIG_SYS_TIMER_DEFINED)
else
$_SRCS-clock     += mb_timers.c
endif

ifdef CONFIG_SYS_INTCRL_DEFINED
$_DEPS-interrupt += $(CONFIG_SYS_INTCRL_DEFINED)
else
$_SRCS-interrupt += mb_irq_ctrl.c
endif
