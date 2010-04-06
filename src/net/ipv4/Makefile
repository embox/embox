
$_MODS += af_inet ipv4 arp icmp udp tcp raw

$_SRCS-af_inet += af_inet.c
$_SRCS-af_inet += protocol.c
$_SRCS-af_inet += route.c
$_SRCS-af_inet += ipconfig.c

$_SRCS-arp     += arp.c
$_SRCS-ipv4    += ip_input.c
$_SRCS-ipv4    += ip_output.c
$_SRCS-icmp    += icmp.c
$_SRCS-udp     += udp.c
$_SRCS-tcp     += tcp.c
$_SRCS-raw     += raw.c

$_DEPS-af_inet += embox.net.socket
$_DEPS-udp     += embox.net.socket
$_DEPS-tcp     += embox.net.socket
$_DEPS-raw     += embox.net.socket
