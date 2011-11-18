
$_PACKAGE = embox.driver.bluetooth.btm

$_MODS += usart
$_SRCS-usart += btm_bluetooth.c
$_DEPS-usart += embox.driver.pins
$_DEPS-usart += embox.kernel.irq
$_REQUIRES-usart += embox.kernel.timer.sleep_api

$_MODS += bt112
$_SRCS-bt112 += btm_bt112.c
$_DEPS-bt112 += usart
$_PROVIDES-bt112 += embox.hal.bluetooth

