
$_MODS += shell

ifdef CONFIG_SHELL_SOURCE

$_SRCS-shell += $(CONFIG_SHELL_SOURCE) shell_command.c console/*.c
$_DEPS-shell += core.kernel.job embox.hal.context

$_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

# In fact only shell.o depends on start_script, but shell.c is acceptible too.
$($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc

else

$_SRCS-shell += shell.c shell_command.c console/*.c
$_DEPS-shell += core.kernel.job embox.hal.context

$_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

# In fact only shell.o depends on start_script, but shell.c is acceptible too.
$($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc

endif

