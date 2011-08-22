$_PACKAGE += embox.kernel.timer

$_APIS += sleep_api
$_APIS += timer_api

$_MODS           += timer
$_SRCS-timer     += timer.c
$_DEPS-timer     += embox.kernel.clock_source
$_PROVIDES-timer += timer_api
$_REQUIRES-timer += embox.hal.clock

$_MODS                += head_timer
$_SRCS-head_timer     += head_timer.c
$_DEPS-head_timer     += embox.kernel.clock_source
$_PROVIDES-head_timer += embox.kernel.timer.timer_api
$_REQUIRES-head_timer += embox.hal.clock

$_MODS           += sleep
$_SRCS-sleep     += sleep.c
$_PROVIDES-sleep += sleep_api
$_REQUIRES-sleep += embox.kernel.timer.timer_api
$_DEPS-sleep     += embox.kernel.thread.core

$_MODS                   += sleep_nosched
$_SRCS-sleep_nosched     += sleep_nosched.c
$_PROVIDES-sleep_nosched += sleep_api
$_REQUIRES-sleep_nosched += embox.kernel.timer.timer_api

