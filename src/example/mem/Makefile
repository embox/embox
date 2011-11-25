$_PACKAGE:= embox.example.mem

$_MODS                   += objalloc
$_SRCS-objalloc          += objalloc.c
$_REQUIRES-objalloc      += embox.mem.objalloc

$_BRIEF-objalloc = demonstrates using of API for allocation fixed size object

$_MODS  += pool
$_SRCS-pool += pool.c
$_DEPS-pool += embox.mem.pool2
$_CPPFLAGS += -I$(SRC_DIR)/include/posix

$_MODS  += page
$_SRCS-page += page.c
$_REQUIRES-page += embox.mem.page_api

$_MODS  += boundary_markers
$_SRCS-boundary_markers += boundary_markers.c
$_REQUIRES-boundary_markers += embox.mem.page_api
