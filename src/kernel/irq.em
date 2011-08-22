
$_PACKAGE := embox.kernel

$_APIS     += irq_api# TODO It seems to be irq_critical. -- Eldar

$_MODS     += irq
$_SRCS-irq += irq.c
$_SRCS-irq += irq_critical.[c|h]
$_PROVIDES-irq += irq_api
$_REQUIRES-irq += embox.hal.interrupt

$_MODS              += irq_critical
$_SRCS-irq_critical += irq_critical.[c|h]
$_PROVIDES-irq_critical += irq_api

