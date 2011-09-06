$_PACKAGE := embox.kernel

# XXX make it a module -- Eldar
$_LIBS     += libcritical.a
$_SRCS-libcritical.a += critical.c
$_CPPFLAGS-libcritical.a += -I$(SRC_DIR)/include/posix
