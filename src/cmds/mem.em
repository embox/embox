$_MODS += mem
$_SRCS-mem += mem.c
$_USES-mem += libframework.a

$_BRIEF-mem = Prints the contents of memory

define $_DETAILS-mem
	NAME
		mem - reads from memory
	SYNOPSIS
		mem [-h] -a addr [-n bytes]
	DESCRIPTION
		Read from memory the specified number of bytes.
	OPTIONS
		-h
			Shows usage
		-a addr
			The target memory address
		-n bytes
			Number of bytes to read
	EXAMPLES
		mem -a 0x40000000 -n 32
	SEE ALSO
		wmem, testmem
	AUTHORS
		Alexey Fomin
endef
