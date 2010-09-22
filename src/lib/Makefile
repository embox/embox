
# Do not process sub-lib makefiles
# may be this is not the best solution, but just for now... -- Eldar
SUBDIRS :=

$_LIBS += libc.a

$_SRCS-libc.a += */*.c

# all that written below don't work
$_DEPS-libc.a += core.kernel.drv_subsys
# Need for stdio

ifdef CONFIG_HARD_UART_OUT
  ifdef CONFIG_HARD_DIAGUART
    $_DEPS-libc.a += embox.driver.diag_ambapp
  else
    $_DEPS-libc.a += embox.driver.ambapp
  endif
else
  ifdef CONFIG_DRIVER_SUBSYSTEM
    $_DEPS-libc.a += core.kernel.drv_subsys
  else
    #default
    $_DEPS-libc.a += core.kernel.diag_ambapp
  endif
endif


