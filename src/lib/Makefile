
# Do not process sub-lib makefiles
# may be this is not the best solution, but just for now... -- Eldar
SUBDIRS :=

$_LIBS += libc.a

$_SRCS-libc.a += */*.c
ifndef CONFIG_NDEBUG
$_SRCS-libc.a += assert.c
endif
