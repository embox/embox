
$_MODS += socket af_packet af_unix

$_SRCS-socket += socket.c socket_allocator.c socket_repo.c
$_SRCS-socket += kernel_socket.c
$_SRCS-socket += port.c
$_DEPS-socket += embox.net.core


$_SRCS-af_packet += af_packet.c
$_DEPS-af_packet += socket

$_SRCS-af_unix += af_unix.c
$_DEPS-af_unix += socket
