
$_MODS += ping
$_SRCS-ping += ping.c
$_DEPS-ping += embox.net.icmp embox.net.socket
$_BRIEF-ping = Send ICMP ECHO_REQUEST to network hosts

define $_DETAILS-ping
	NAME
		ping - send ICMP ECHO_REQUEST to network hosts
	SYNOPSIS
		ping [-I iface] [-c count] [-W timeout] [-t ttl]
		[-p pattern] [-t ttl] [-s packsize] [ -i interval]
		destination
	DESCRIPTION
		ping uses the ICMP protocol s mandatory ECHO_REQUEST
		datagram to elicit an ICMP ECHO_RESPONSE from a host
		or gateway.
		Test whether a particular host is reachable
	OPTIONS
		-I interface
			Set source address to specified interface address.
			by default: eth0
		-c count
			Stop after sending count ECHO_REQUEST packets.
			by default: 4 (-1 infinity)
		-W timeout
			Time to wait for a response, in seconds.
			by default: 1 second
		-t ttl
			Set the IP Time to Live.
		-s packetsize
			Specifies  the number of data bytes to be sent.
			The default is 56.
		-i interval
			Wait  interval seconds between sending each packet.
			The default is to wait for 1 second
		-p pattern
			16 ``pad'' bytes to fill out the packet you send.
	EXAMPLES
		ping -c 3 10.0.3.97
		This will send 3 ECHO_REQUEST to 10.0.3.97
	SEE ALSO
		ifconfig
	AUTHORS
		Alexandr Batyukov
		Anton Bondarev
		Nikolay Korotky
endef
