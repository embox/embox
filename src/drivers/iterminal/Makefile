
#$_MODS += iterminal
#$_SRCS-iterminal += iterminal.c iterminal_main.c #shell.c shell_command.c #console/*.c
#$_DEPS-iterminal += core.kernel.drv_subsys core.kernel.thread
#$_DEPS-iterminal += core.kernel.scheduler embox.driver.diag_ambapp

# In fact only shell.o depends on start_script, but shell.c is acceptible too.
#$($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc

$_MODS += esh
$_SRCS-esh += esh.c
$_DEPS-esh += core.kernel.printk #core.kernel.job embox.hal.context

