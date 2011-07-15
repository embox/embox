$_PACKAGE += core.kernel.timer

$_APIS += sleep_api

$_MODS           += timer
$_SRCS-timer     += timer.c
$_DEPS-timer     += core.kernel.clock_source
$_REQUIRES-timer += embox.hal.clock

$_MODS           += sleep
$_SRCS-sleep     += sleep.c
$_PROVIDES-sleep += sleep_api
$_DEPS-sleep     += timer
$_DEPS-sleep     += core.kernel.thread.core

$_MODS                   += sleep_nosched
$_SRCS-sleep_nosched     += sleep_nosched.c
$_PROVIDES-sleep_nosched += sleep_api
$_DEPS-sleep_nosched     += timer
