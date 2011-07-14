
SUBDIRS :=

$_LIBS += libframework.a
$_USES-libframework.a += libc.a

$_SRCS-libframework.a += */*/*.c */*.c *.c

$_CPPFLAGS-libframework.a += -D__FRAMEWORK__
$_CPPFLAGS-libframework.a += -I$(SRC_DIR)/linux-compat/include
