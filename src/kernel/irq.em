
$_PACKAGE := embox.kernel

$_MODS     += irq
$_SRCS-irq += irq.c
$_REQUIRES-irq += embox.hal.mm.objalloc
$_REQUIRES-irq += embox.hal.interrupt

$_LIBS            += libirq_lock.a
$_SRCS-libirq_lock.a += irq_lock.c
$_CPPFLAGS-libirq_lock.a += -I$(SRC_DIR)/include/posix
