$_MODS += arping
$_SRCS-arping += arping.c
$_DEPS-arping += embox.net.arp

$_BRIEF-arping = Send ARP REQUEST to a neighbour host

define $_DETAILS-arping
	NAME
		arping - send ARP REQUEST to a neighbour host
	SYNOPSIS
		arping [ -c count] [-I iface] destination
	DESCRIPTION
		Ping destination on device interface by ARP packets,
		using source address source.
	OPTIONS
		-I interface
			Set source address to specified interface address.
			by default: eth0
		-c count
			Stop after sending count ECHO_REQUEST packets.
			by default: 4 (-1 infinity)
	EXAMPLES
		arping -c 3 10.0.3.97
	SEE ALSO
		arp, ping
	AUTHORS
		Nikolay Korotky
endef
