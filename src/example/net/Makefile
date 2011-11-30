$_PACKAGE:= embox.example.net

$_MODS          += receiver

$_SRCS-receiver += receiver.c
$_DEPS-receiver += embox.net.socket
$_DEPS-receiver += embox.net.eth
$_DEPS-receiver += embox.net.arp

$_MODS              += tcp_receiver
$_SRCS-tcp_receiver += tcp_receiver.c
$_DEPS-tcp_receiver += embox.net.socket
//$_DEPS-receiver += embox.net.eth
//$_DEPS-receiver += embox.net.arp

$_MODS += r6040
$_SRCS-r6040_example  = r6040.c

