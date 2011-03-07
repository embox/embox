
$_MODS += wmem

$_SRCS-wmem += wmem.c
$_USES-wmem += libframework.a

$_BRIEF-wmem = Writes memory word at the specified address

define $_DETAILS-wmem
	NAME
		wmem - writes to memory
	SYNOPSIS
		wmem [-h] -a addr -v value
	OPTIONS
		-h
			Shows usage
		-a addr
			The destination address
		-v value
			The value to write
	EXAMPLES
		wmem -a 0x40000000 -v 0xdeadbeef
	SEE ALSO
		mem
	AUTHORS
		Alexey Fomin
endef
