
# Auto-generated package declaration inherited from parent directory.
# TODO Suggested renaming package to net.ipv4
$_PACKAGE := embox.net

$_MODS         += af_inet
$_SRCS-af_inet += af_inet.c
$_SRCS-af_inet += protocol.c
$_SRCS-af_inet += route.c
$_SRCS-af_inet += ipconfig.c
$_DEPS-af_inet += embox.net.socket
$_DEPS-af_inet += embox.net.raw_sock

$_MODS     += arp
$_SRCS-arp += arp.c
$_DEPS-arp += embox.net.neighbour

$_MODS      += ipv4
$_SRCS-ipv4 += ip_input.c : ip_options.c
$_SRCS-ipv4 += ip_output.c

$_MODS      += icmp
$_SRCS-icmp += icmp.c
$_DEPS-icmp += af_inet

$_MODS     += udp
$_SRCS-udp += udp.c
$_DEPS-udp += embox.net.socket
$_DEPS-udp += embox.net.udp_sock

$_MODS     += tcp
$_SRCS-tcp += tcp.c
$_DEPS-tcp += embox.net.socket
$_DEPS-tcp += embox.net.tcp_sock

$_MODS          += raw_sock
$_SRCS-raw_sock += raw_sock.c
$_DEPS-raw_sock += embox.net.socket

$_MODS          += tcp_sock
$_SRCS-tcp_sock += tcp_sock.c
$_DEPS-tcp_sock += embox.net.socket

$_MODS          += udp_sock
$_SRCS-udp_sock += udp_sock.c
$_DEPS-udp_sock += embox.net.socket
