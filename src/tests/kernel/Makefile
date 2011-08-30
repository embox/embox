
$_PACKAGE := embox.test.kernel

$_MODS               += irq_test
$_SRCS-irq_test      += irq_test.c
$_DEPS-irq_test      += embox.kernel.irq
$_REQUIRES-irq_test  += embox.hal.interrupt

$_MODS             += softirq_test
$_SRCS-softirq_test = softirq_test.c
$_DEPS-softirq_test = embox.kernel.softirq

$_MODS       += timer
$_SRCS-timer += timer.c
$_REQUIRES-timer += embox.kernel.timer.timer_api
