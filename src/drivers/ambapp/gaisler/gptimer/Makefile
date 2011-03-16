$_PACKAGE := embox.driver.clock

$_MODS             += gptimer
$_SRCS-gptimer     += gptimer.c
$_PROVIDES-gptimer += embox.hal.clock

ifdef CONFIG_AMBAPP
  $_DEPS-gptimer += embox.driver.ambapp
endif
