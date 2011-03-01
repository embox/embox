
# Fuck, this is a hell, not shell! =\ -- Eldar

$_MODS += shell

ifeq ($(CONFIG_SHELL_SOURCE:shell.c=),) # old

    $_SRCS-shell += shell.c console/*.c
#    $_DEPS-shell += \
       core.kernel.job \
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
       core.kernel.job \
       embox.hal.context \
       core.kernel.drv_subsys \
       core.kernel.scheduler \
       core.kernel.thread

endif
