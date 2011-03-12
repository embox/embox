$_MODS += tftp
$_SRCS-tftp += tftp.c
$_DEPS-tftp += embox.net.udp
$_DEPS-tftp += embox.fs.core

$_BRIEF-tftp = TFTP client

define $_DETAILS-tftp
	NAME
		tftp - IPv4 Trivial File Transfer Protocol client
	SYNOPSIS
		tftp <host> <file>
	DESCRIPTION
		tftp is a client for the IPv4 Trivial file Transfer
		Protocol, which can be used to transfer files from
		remote machines.
	OPTIONS
		<host> - address of the server.
		<file> - name of the file to Receive.
	EXAMPLES
		tftp 192.168.0.59 image
	SEE ALSO
		boot, goto
	AUTHORS
		Andrey Baboshin, Nikolay Korotky
endef
