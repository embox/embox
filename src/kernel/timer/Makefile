$_PACKAGE += core.kernel.timer

$_APIS += sleep_api
$_APIS += timer_api

$_MODS           += timer
$_SRCS-timer     += timer.[c|h]
$_DEPS-timer     += core.kernel.clock_source
$_PROVIDES-timer += timer_api
$_REQUIRES-timer += embox.hal.clock

$_MODS                += head_timer
$_SRCS-head_timer     += head_timer.[c|h]
$_DEPS-head_timer     += core.kernel.clock_source
$_PROVIDES-head_timer += core.kernel.timer.timer_api
$_REQUIRES-head_timer += embox.hal.clock

$_MODS           += sleep
$_SRCS-sleep     += sleep.c
$_PROVIDES-sleep += sleep_api
$_REQUIRES-sleep += core.kernel.timer.timer_api
$_DEPS-sleep     += core.kernel.thread.core

$_MODS                   += sleep_nosched
$_SRCS-sleep_nosched     += sleep_nosched.c
$_PROVIDES-sleep_nosched += sleep_api
$_REQUIRES-sleep_nosched += core.kernel.timer.timer_api
$_DEPS-sleep     += core.kernel.thread.core

