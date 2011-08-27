$_PACKAGE := embox.driver.clock

$_MODS               += mb_timer
$_SRCS-mb_timer      += mb_timer.c
$_DEPS-mb_timer      += embox.kernel.irq
$_PROVIDES-mb_timer  += embox.hal.clock

$_MODS               += at91_pitc
$_SRCS-at91_pitc     += at91_pitc.c
$_DEPS-at91_pitc     += embox.kernel.irq
$_PROVIDES-at91_pitc += embox.hal.clock

$_MODS               += pit
$_SRCS-pit           += pit.c
$_DEPS-pit           += embox.kernel.clock_source
$_DEPS-pit           += embox.kernel.irq
$_PROVIDES-pit       += embox.hal.clock

$_MODS               += gptimer
$_SRCS-gptimer       += gptimer.c
$_DEPS-gptimer       += embox.kernel.irq
$_PROVIDES-gptimer   += embox.hal.clock
ifdef CONFIG_AMBAPP
  $_DEPS-gptimer     += embox.driver.ambapp
endif
