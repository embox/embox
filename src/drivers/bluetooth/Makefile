$_APIS += embox.hal.bluetooth

$_MODS += nxt_bluetooth btm_bluetooth
$_SRCS-nxt_bluetooth += nxt_bluetooth_usart_new.c nxt_bluetooth_assist_new.c
$_DEPS-nxt_bluetooth += embox.driver.pins
$_DEPS-nxt_bluetooth += embox.kernel.irq
$_PROVIDES-nxt_bluetooth += embox.hal.bluetooth
$_REQUIRES-nxt_bluetooth += embox.kernel.timer.sleep_api

$_SRCS-btm_bluetooth += btm_bluetooth.c
$_DEPS-btm_bluetooth += embox.driver.pins
$_DEPS-btm_bluetooth += embox.kernel.irq
$_PROVIDES-btm_bluetooth += embox.hal.bluetooth
$_REQUIRES-btm_bluetooth += embox.kernel.timer.sleep_api
