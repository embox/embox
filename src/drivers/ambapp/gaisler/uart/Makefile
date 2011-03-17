$_PACKAGE := embox.driver.serial

$_MODS                  += diag_apbuart
$_SRCS-diag_apbuart     += diag_apbuart.c
$_PROVIDES-diag_apbuart += embox.hal.diag

$_MODS           += apbuart
$_SRCS-apbuart   += uart.c
ifdef CONFIG_AMBAPP
  $_DEPS-apbuart += embox.driver.ambapp
endif
