$_PACKAGE := embox.driver.video

$_MODS += vga_console vga_console_tty
$_SRCS-vga_console += vga_console.c
$_SRCS-vga_console_tty += tty_device.c
$_DEPS-vga_console_tty += embox.kernel.irq

$_DEPS-vga_console += embox.driver.keyboard
$_PROVIDES-vga_console += embox.hal.diag
