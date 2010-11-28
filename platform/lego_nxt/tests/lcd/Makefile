
$_CPPFLAGS += -I$($_PLATFORM)/include
$_MODS += nxt_lcd nxt_lcd_picture nxt_splash nxt_fill
$_SRCS-nxt_lcd += lcd.c
$_DEPS-nxt_lcd += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_lcd_picture += picture_embox.c
$_DEPS-nxt_lcd_picture += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_splash += abc_embox.c
$_DEPS-nxt_splash += $(PLATFORM).driver.nxt_lcd

$_SRCS-nxt_fill += test_fill.c
$_DEPS-nxt_fill += $(PLATFORM).driver.nxt_lcd
