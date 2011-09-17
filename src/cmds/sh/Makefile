$_PACKAGE := embox.cmd.sh


$_APIS += shell_api

$_MODS += tish
$_SRCS-tish += tish.c
$_CPPFLAGS-tish += -I$(AUTOCONF_DIR)
$_PROVIDES-tish += shell_api

$_MODS += shell_start
$_SRCS-shell_start += shell_start.c
$_REQUIRES-shell_start += shell_api
