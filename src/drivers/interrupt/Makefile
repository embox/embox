$_PACKAGE := embox.driver.interrupt

$_MODS               += mb_intc
$_SRCS-mb_intc       += mb_intc.c
$_PROVIDES-mb_intc   += embox.hal.interrupt

$_MODS               += at91_aic
$_SRCS-at91_aic      += at91_aic.c
$_PROVIDES-at91_aic  += embox.hal.interrupt

$_MODS               += apic
$_SRCS-apic          += apic.c
$_PROVIDES-apic      += embox.hal.interrupt

$_MODS               += irqmp
$_SRCS-irqmp         += irqmp.c
$_PROVIDES-irqmp     += embox.hal.interrupt
ifdef CONFIG_AMBAPP
  $_DEPS-irqmp       += embox.driver.ambapp
endif
