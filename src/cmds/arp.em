
$_MODS += arp
$_SRCS-arp += arp.c
$_DEPS-arp += embox.net.arp

$_BRIEF-arp = Manipulate the system ARP cache

define $_DETAILS-arp
	NAME
	    	arp - manipulate the system ARP cache.
	SYNOPSIS
		arp [-hds] [-a addr] [-m addr] [-i ifs]
	DESCRIPTION
		arp with no mode specifier will print the
			current content of the table.
	OPTIONS
		-i - select  an  interface
		-d - will delete a ARP table entry
		-s - used to set up a new table entry
		-a - ip address
		-m - mac address
		-h - show this help
	EXAMPLES
		arp -s -a 10.0.3.97 -m 11:22:AA:44:FF:11 -i eth0
			This will add new entry to ARP table.
		arp -d -a 10.0.0.2
			Delete the ARP table entry for 10.0.0.2.
	SEE ALSO
		ifconfig, route
	AUTHORS
		Nikolay Korotky
endef
