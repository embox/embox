
$_MODS_CORE += locore context

$_SRCS-locore  += boot.S
$_SRCS-locore  += idt.c boot_idt.S irq_handler.c
$_SRCS-locore  += gdt.c gdt_flush.S
$_SRCS-context += context.c context_switch.S
$_SRCS-arch    += arch.c

include $(MK_DIR)/system.mk

