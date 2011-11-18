
$_PACKAGE = embox.driver.bluetooth.lego

$_MODS += usart
$_SRCS-usart += nxt_bluetooth_usart.c
$_DEPS-usart += embox.driver.pins
$_DEPS-usart += embox.kernel.irq
$_REQUIRES-usart += embox.kernel.timer.sleep_api

$_MODS += blue_core
$_SRCS-blue_core += blue_core4.c
$_DEPS-blue_core += usart
$_PROVIDES-blue_core += embox.hal.bluetooth

