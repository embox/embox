$_PACKAGE := embox.driver.serial

$_MODS += apbuart diag_apbuart

$_SRCS-apbuart += uart.c
$_SRCS-diag_apbuart += diag_apbuart.c

ifdef CONFIG_AMBAPP
  $_DEPS-apbuart += embox.driver.ambapp
endif
