$_PACKAGE := embox.driver.serial

$_APIS += embox.driver.serial

$_MODS                 += xuartlite
$_SRCS-xuartlite       += serial_xuartlite.c
$_DEPS-xuartlite       += embox.kernel.irq
$_PROVIDES-xuartlite   += embox.driver.serial

$_MODS                 += at91_uart
$_SRCS-at91_uart       += at91_uart.c
$_PROVIDES-at91_uart   += embox.driver.serial

$_MODS                 += skyeye_uart
$_SRCS-skyeye_uart     += skyeye_uart.c
$_PROVIDES-skyeye_uart += embox.driver.serial

$_MODS                 += serial_port
$_SRCS-serial_port     += serial_port.c
$_DEPS-serial_port     += embox.kernel.irq
$_PROVIDES-serial_port += embox.driver.serial


$_MODS           	+= apbuart
$_SRCS-apbuart  	+= apbuart.c
$_DEPS-apbuart   	+= embox.kernel.irq
$_PROVIDES-apbuart  += embox.driver.serial

ifdef CONFIG_FS
	$_DEPS-apbuart += embox.fs.devfs
endif

ifdef CONFIG_AMBAPP
  $_DEPS-apbuart += embox.driver.ambapp
endif
