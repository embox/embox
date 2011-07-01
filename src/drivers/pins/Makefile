
$_MODS += at91_pins
$_SRCS-at91_pins += at91_pins.c
$_DEPS-at91_pins += core.kernel.irq
$_PROVIDES-at91_pins += embox.hal.pins

$_MODS += pins
$_SRCS-pins += pins.c
$_DEPS-pins += $(CONFIG_PIN_DRIVER)
$_REQUIRES-pins += embox.hal.pins
