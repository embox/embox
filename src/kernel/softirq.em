
$_PACKAGE := embox.kernel

$_APIS += softirq_api# TODO Bad name.

$_MODS         += softirq
$_SRCS-softirq += softirq.c
$_PROVIDES-softirq += softirq_api
# FIXME to be removed.
$_REQUIRES-softirq += embox.kernel.timer.timer_api

$_MODS                  += softirq_critical
$_SRCS-softirq_critical += softirq_critical.h
$_PROVIDES-softirq_critical += softirq_api

