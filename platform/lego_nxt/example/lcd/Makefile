$_MODS += nxt_lcd_picture
$_MODS += nxt_splash
$_MODS += nxt_fill
$_MODS += nxt_new_fill
$_MODS += nxt_lcd_picture_fill
$_MODS += nxt_lcd_picture_embox_fill
$_MODS += nxt_print_test

$_SRCS-nxt_lcd_picture += picture_embox.c
$_DEPS-nxt_lcd_picture += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_splash += abc_embox.c
$_DEPS-nxt_splash += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_fill += test_fill.c
$_DEPS-nxt_fill += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_new_fill += new_fill.c
$_DEPS-nxt_new_fill += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_lcd_picture_fill += picture_embox_fill.c
$_DEPS-nxt_lcd_picture_fill += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_lcd_picture_embox_fill += picture_embox_with_fill.c
$_DEPS-nxt_lcd_picture_embox_fill += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_print_test += print_test.c
$_DEPS-nxt_print_test += $(PLATFORM).driver.nxt_lcd
