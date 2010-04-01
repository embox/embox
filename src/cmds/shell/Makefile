
$_MODS += shell

$_SRCS-shell += *.c console/*.c

$_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

# In fact only shell.o depends on start_script, but shell.c is acceptible too.
$($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc
