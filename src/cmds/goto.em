
$_MODS += goto

$_SRCS-goto += goto.c

$_DEPS-goto += embox.hal.interrupt

$_BRIEF-goto = Transfer of control flow

define $_DETAILS-goto
	NAME
		goto - one-way control transfer
	SYNOPSIS
		goto [-h] [-a address]
	DESCRIPTION
		Executes the code at the specified address.
	OPTIONS
		-h
			Show usage
		-a address
			Entry point
	EXAMPLES
		goto -a 0x40000000
	SEE ALSO
		load
	AUTHORS
		Sergey Kuzmin
endef
