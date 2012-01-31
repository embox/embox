$_CPPFLAGS += -I$(SRC_DIR)/compat/posix/include

$_PACKAGE := embox.prom

$_APIS += printf

$_MODS += prom_printf
$_SRCS-prom_printf += prom_printf.[c|h]
$_PROVIDES-prom_printf  += embox.prom.printf
$_REQUIRES-prom_printf += embox.driver.diag.diag_api

$_MODS += noprom_printf
$_SRCS-noprom_printf += noprom_printf.h
$_PROVIDES-noprom_printf  += embox.prom.printf



