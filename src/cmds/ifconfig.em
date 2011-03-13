$_MODS += ifconfig
$_SRCS-ifconfig += ifconfig.c
$_DEPS-ifconfig += embox.net.core

$_BRIEF-ifconfig = Configure a network interface

define $_DETAILS-ifconfig
	NAME
		ifconfig - configure a network interface
	SYNOPSIS
		ifconfig [-a ipaddr] [-m macaddr] [-p mask] [-u|d]
			[-r arp] [-f promisc] [-c allmulti]
			[-g multicast] [-l mtu] [-b address]
			[-i irq_num] [-t maxtxqueue] [-w brdcstaddr]
			[-z p2p][[-x] name|interface]
	DESCRIPTION
		Ifconfig is used to configure the kernel-resident
		network interfaces. If no arguments are given, ifconfig
		displays the status of the currently active interfaces.
		command changes ip/mac addresses of specified interface.
	OPTIONS
		-a addr	The IP address to be assigned to this iface.
		-p mask	The IP mask.
		-m addr	The MAC address to be assigned to this iface.
		-u 	Up interface.
		-d 	Down interface.
		-x 	Find interface by name.
		-r 	Enable arp protocol using.
		-f 	Promiscuous state control.
		-c 	All-multicast state control.
		-g 	Control flag for group data transfer support.
		-l 	Set max packet length.
		-b 	Set iface base address.
		-i 	Set iface irq.
		-t 	Set max queue length.
		-w 	The broadcast address to be assigned.
		-z 	Pointipoint connection control.
	EXAMPLES
		ifconfig -a 10.0.0.1 -m 11:AA:22:CC:FF:00 -u eth0
			This will setup new interface eth0.
	SEE ALSO
		route, arp
	AUTHORS
		Anton Bondarev, Alexandr Batyukov
endef
