
$_MODS += route
$_SRCS-route += route.c
$_DEPS-route += embox.net.ipv4
$_BRIEF-route = Manipilate routing table

define $_DETAILS-route
	NAME
		route - show / manipulate the IP routing table.
	SYNOPSIS
		route  -n <net> -m <mask> -d <name> [add|del]
	DESCRIPTION
		del    delete a route.
		add    add a new route.
		-n <network>
		-m <mask>
		-g <gateway>
		-d <device>
	SEE ALSO
		ifconfig, arp
	AUTHOR
		Nikolay Korotky
endef
