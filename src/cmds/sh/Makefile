$_PACKAGE := embox.cmd.sh


$_APIS += shell_api

$_MODS += tish
$_SRCS-tish += tish.c
$_PROVIDES-tish += shell_api
$_DEPS-tish += embox.compat.posix

$_MODS += shell_start
$_SRCS-shell_start += shell_start.c
$_CPPFLAGS-shell_start += -I$(AUTOCONF_DIR)
$_REQUIRES-shell_start += shell_api

$($_SELFDIR)/shell_start.c : $(AUTOCONF_DIR)/start_script.inc
