
$_PACKAGE := embox.kernel

$_MODS      += time
$_SRCS-time += time.c
$_DEPS-time += embox.kernel.clock_source
$_REQUIRES-time += embox.kernel.timer.timer_api

$_MODS                  += clock_source
$_SRCS-clock_source     += clock_source.c
