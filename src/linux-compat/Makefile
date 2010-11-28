
SUBDIRS := $(filter-out include,$(SUBDIRS))

$_LIBS += liblinux.a
$_USES-liblinux.a += libc.a

$_CPPFLAGS-liblinux.a += -I$($_SELFDIR)/include
