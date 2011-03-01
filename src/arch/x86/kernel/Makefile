
$_MODS_CORE += locore context

$_SRCS-locore  += boot.S
$_SRCS-locore  += idt.c boot_idt.S irq_handler.c
$_SRCS-locore  += gdt.c gdt_flush.S
$_SRCS-context += context.c context_switch.S
$_SRCS-arch    += arch.c

DEFAULT_SYS_TIMER = embox.driver.timers.pit_timer
DEFAULT_SYS_IRQCTRL = embox.driver.interrupts.apic_interrupt

include $(MK_DIR)/system.mk

