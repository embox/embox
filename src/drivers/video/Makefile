$_PACKAGE := embox.driver.video

$_MODS += vga_console
$_SRCS-vga_console += vga_console.c
ifdef CONFIG_TTY_DEVICE
$_SRCS-vga_console += tty_device.c
endif
$_DEPS-vga_console += embox.driver.keyboard


