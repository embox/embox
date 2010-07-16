
$_MODS += shell

ifdef CONFIG_SHELL_SOURCE

  ifeq ($(CONFIG_SHELL_SOURCE), shell.c) # old

    $_SRCS-shell += shell.c shell_command.c console/*.c
#    $_DEPS-shell += core.kernel.job embox.hal.context
    $_DEPS-shell += core.kernel.job embox.hal.context embox.driver.apbuart embox.driver.diag_apbuart

    $_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

    # In fact only shell.o depends on start_script, but shell.c is acceptible too.
    $($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc


  else # new

    $_SRCS-shell += $(CONFIG_SHELL_SOURCE) shell_command.c
    $_DEPS-shell += core.kernel.job embox.hal.context core.kernel.drv_subsys

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
