
$_SUBDIRS := $(ARCH)

$_PACKAGE := embox.hal

$_CPPFLAGS += -D__HAL__

# Each HAL implementation should provide the following mods definition.
# Kernel assumes their presense.
$_MODS += arch clock interrupt context
$_MODS += traps

# Overrides some libc routines with optimized ones.
$_LIBS += libarch.a
# Force libarch.a to be linked into the image prior to default libc.a
$_SUBS-libarch.a += libc.a
