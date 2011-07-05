#ifdef CONFIG_TIMER
#  $_SRCS-timer     += timer.c
#  $_REQUIRES-timer += embox.hal.clock
#endif

$_PACKAGE += core.kernel.timer

$_APIS += api

$_MODS += without_threads
$_SRCS-without_threads += ot_timer.c
$_REQUIRES-without_threads += embox.hal.clock
$_PROVIDES-without_threads += core.kernel.timer.api


$_MODS += multi_threads
$_SRCS-multi_threads += mt_timer.c
$_REQUIRES-multi_threads += embox.hal.clock
$_PROVIDES-multi_threads += core.kernel.timer.api

