
# Auto-generated package declaration inherited from parent directory.
# TODO Suggested renaming package to cmds.shell
$_PACKAGE := embox.cmd


# Fuck, this is a hell, not shell! =\ -- Eldar

$_MODS += shell

ifeq ($(CONFIG_SHELL_SOURCE:shell.c=),) # old

    $_SRCS-shell += shell.c console/*.c
#    $_DEPS-shell += \
       embox.kernel.job \
       embox.hal.context \
       embox.driver.apbuart \
       embox.driver.diag_apbuart
    $_USES-shell += libframework.a

    $_CPPFLAGS-shell += -I$(AUTOCONF_DIR)

    # In fact only shell.o depends on start_script, but shell.c is acceptible too.
    $($_SELFDIR)/shell.c : $(AUTOCONF_DIR)/start_script.inc

else # new

    $_SRCS-shell += $(CONFIG_SHELL_SOURCE)
    $_DEPS-shell += \
       embox.kernel.job \
       embox.hal.context \
       embox.kernel.drv_subsys \
       embox.kernel.scheduler \
       embox.kernel.thread

endif

$_DEPS-shell += embox.compat.posix
$_DEPS-shell += embox.driver.terminal
