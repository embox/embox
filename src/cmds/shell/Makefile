
$_MODS += shell

ifdef CONFIG_SHELL_SOURCE

ifdef CONFIG_HARD_UART_OUT # old or new version shell with hard out to uart

$_SRCS-shell += $(CONFIG_SHELL_SOURCE) shell_command.c console/*.c
$_DEPS-shell += core.kernel.job embox.hal.context embox.driver.apbuart embox.driver.diag_apbuart

$_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

# In fact only shell.o depends on start_script, but shell.c is acceptible too.
$($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc

else # only new shell without hard output ------------------------------------

$_SRCS-shell += $(CONFIG_SHELL_SOURCE) shell_command.c
$_DEPS-shell += core.kernel.job embox.hal.context

$_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

# In fact only shell.o depends on start_script, but shell.c is acceptible too.
$($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc

endif

else # oldest version of shell -----------------------------------------------

$_SRCS-shell += shell.c shell_command.c console/*.c
$_DEPS-shell += core.kernel.job embox.hal.context

$_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

# In fact only shell.o depends on start_script, but shell.c is acceptible too.
$($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc

endif

