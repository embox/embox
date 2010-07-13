
# Do not process sub-lib makefiles
# may be this is not the best solution, but just for now... -- Eldar
$_SUBDIRS :=

$_LIBS += libc.a

$_SRCS-libc.a += */*.c

# Need for stdio
$_DEPS-libc.a += core.kernel.drv_subsys
