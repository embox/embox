$_APIS += embox.hal.bluetooth

$_MODS += nxt_bluetooth
$_SRCS-nxt_bluetooth += nxt_bluetooth_usart.c
$_DEPS-nxt_bluetooth += embox.driver.pins
$_DEPS-nxt_bluetooth += embox.kernel.irq
$_PROVIDES-nxt_bluetooth += embox.hal.bluetooth
$_REQUIRES-nxt_bluetooth += embox.kernel.timer.sleep_api

$_MODS += blue_core
$_SRCS-blue_core += blue_core4.c
$_DEPS-blue_core += nxt_bluetooth

$_MODS += btm_bluetooth

$_SRCS-btm_bluetooth += btm_bluetooth.c
$_DEPS-btm_bluetooth += embox.driver.pins
$_DEPS-btm_bluetooth += embox.kernel.irq
$_PROVIDES-btm_bluetooth += embox.hal.bluetooth
$_REQUIRES-btm_bluetooth += embox.kernel.timer.sleep_api
