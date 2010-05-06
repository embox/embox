$_PACKAGE := embox.driver.timers

$_MODS += gptimer

$_SRCS-gptimer += clock_gptimer.c

ifdef CONFIG_AMBAPP
  $_DEPS-gptimer += embox.driver.ambapp
endif
