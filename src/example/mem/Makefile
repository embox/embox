$_PACKAGE:= embox.example.mem

$_MODS                   += objalloc
$_SRCS-objalloc          += objalloc.c
$_REQUIRES-objalloc      += embox.hal.mm.objalloc

$_BRIEF-objalloc = demonstrates using of API for allocation fixed size object

$_MODS  += pool
$_SRCS-pool += pool.c
$_DEPS-pool += embox.hal.mm.pool2
$_CPPFLAGS += -I$(SRC_DIR)/include/posix

$_MODS  += page
$_SRCS-page += page.c
