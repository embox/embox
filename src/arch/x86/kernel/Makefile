
$_MODS_CORE += locore context

$_SRCS-locore  += boot.S
$_SRCS-locore  += idt.c boot_idt.S
$_SRCS-locore  += gdt.c
$_SRCS-context += context.c
$_SRCS-arch    += arch.c

DEFAULT_SYS_IRQCTRL = embox.driver.interrupts.apic_interrupt

include $(MK_DIR)/system.mk

