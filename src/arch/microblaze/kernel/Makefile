
$_MODS_CORE += locore context

$_SRCS-prom += open_prom.c

$_SRCS-locore += interrupt_handler.c sys.c ivt.S entry.S boot.S mb_trap_dispatcher.c

$_SRCS-arch += arch.c
$_SRCS-context += context.c
$_SRCS-traps += traps_core.c

DEFAULT_SYS_MOD_DIAG = embox.driver.serial.xuartlite

include $(MK_DIR)/system.mk


