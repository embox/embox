$_PACKAGE := embox.driver.video

#TODO embox.hal.driver.vga bad style fix it
$_APIS += embox.hal.driver.vga

$_MODS += vga_console_tty

$_SRCS-vga_console_tty += tty_device.c
$_DEPS-vga_console_tty += embox.kernel.irq
$_MODS += vga_console
$_SRCS-vga_console += vga_console.c
$_PROVIDES-vga_console += embox.hal.driver.vga
