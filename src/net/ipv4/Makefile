$_MODS += af_inet ipv4 arp icmp udp tcp raw_sock tcp_sock udp_sock

$_SRCS-af_inet  += af_inet.c
$_SRCS-af_inet  += protocol.c
$_SRCS-af_inet  += route.c
$_SRCS-af_inet  += ipconfig.c

$_SRCS-arp      += arp.c
$_SRCS-ipv4     += ip_input.c : ip_options.c
$_SRCS-ipv4     += ip_output.c
$_SRCS-icmp     += icmp.c
$_SRCS-udp      += udp.c
$_SRCS-tcp      += tcp.c
$_SRCS-raw_sock += raw_sock.c
$_SRCS-tcp_sock += tcp_sock.c
$_SRCS-udp_sock += udp_sock.c

$_DEPS-af_inet  += embox.net.socket
$_DEPS-af_inet  += embox.net.raw_sock
$_DEPS-udp      += embox.net.socket
$_DEPS-tcp      += embox.net.socket
$_DEPS-raw_sock += embox.net.socket

$_DEPS-tcp      += tcp_sock
$_DEPS-tcp_sock += embox.net.socket
$_DEPS-udp      += udp_sock
$_DEPS-udp_sock += embox.net.socket
