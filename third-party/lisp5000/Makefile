$_PACKAGE := org.lisp500

$_CPPFLAGS += -I$(SRC_DIR)/compat/posix/include

$_MODS += lisp500_core
$_SRCS-lisp500_core += lisp500.c
$_REQUIRES-lisp500_core += embox.mem.heap_api

$_ROOTFS_SRCS := $(SELFDIR)/init500lite.lisp $(SELFDIR)/fib.lisp
