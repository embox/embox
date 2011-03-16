
SUBDIRS := $(ARCH)

$_PACKAGE := embox.hal

$_CPPFLAGS += -D__HAL__

$_APIS += interrupt clock

$_MODS += arch context
$_MODS += traps prom

# Overrides some libc routines with optimized ones.
$_LIBS += libarch.a
# Force libarch.a to be linked into the image prior to default libc.a
$_SUBS-libarch.a += libc.a

