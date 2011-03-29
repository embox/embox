$_PACKAGE := embox.driver.serial

$_MODS                 += xuartlite
$_SRCS-xuartlite       += serial_xuartlite.c
$_PROVIDES-xuartlite   += embox.hal.diag

$_MODS                 += at91_uart
$_SRCS-at91_uart       += at91_uart.c
$_PROVIDES-at91_uart   += embox.hal.diag

$_MODS                 += skyeye_uart
$_SRCS-skyeye_uart     += skyeye_uart.c
$_PROVIDES-skyeye_uart += embox.hal.diag

$_MODS                 += serial_port
$_SRCS-serial_port     += serial_port.c
$_PROVIDES-serial_port += embox.hal.diag

$_MODS                  += diag_apbuart
$_SRCS-diag_apbuart     += diag_apbuart.c
$_PROVIDES-diag_apbuart += embox.hal.diag

$_MODS           += apbuart
$_SRCS-apbuart   += apbuart.c
ifdef CONFIG_AMBAPP
  $_DEPS-apbuart += embox.driver.ambapp
endif
