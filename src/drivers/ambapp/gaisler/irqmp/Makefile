$_PACKAGE := embox.driver.interrupt

$_MODS               += apb_irqmp
$_PROVIDES-apb_irqmp += embox.hal.interrupt
$_SRCS-apb_irqmp     += apb_irqmp.c

ifdef CONFIG_AMBAPP
  $_DEPS-apb_irqmp += embox.driver.ambapp
endif
