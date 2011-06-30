# Just to suppress build system warnings.

$_LIBS += libutil.a

ifndef CONFIG_LIST_NDEBUG
$_SRCS-libutil.a += list_debug.c
endif
ifndef CONFIG_SLIST_NDEBUG
$_SRCS-libutil.a += slist_debug.c
endif
$_SRCS-libutil.a += pool.c

$_USES-libutil.a += libc.a
