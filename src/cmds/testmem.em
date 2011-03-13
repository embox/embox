$_MODS += testmem
$_SRCS-testmem += testmem.c memory_tests.c

$_BRIEF-testmem = Set of memory tests

define $_DETAILS-testmem
	NAME
		testmem - set of memory tests
	SYNOPSIS
		testmem [-h] [-a <addr>] [-n <number>] [-t <type of test]
	DESCRIPTION
		Read from memory or test memory.
	OPTIONS
		-h -  to see this page
		-a addr
    			 memory region start addr
		-n number of bytes (default 1000000)
		    in case of loop:
    	            number of times starting test
    	            0 - infinite (default)
    		    memory region size
    		-t - test type
    			runzero
    			runone
    			!n/a! address
    			!n/a! chess
    			loop
	EXAMPLES
		testmem -a 0x40000000 -c 0x10000 -t loop
		testmem -a 0x40000000 -c 0x10000 -t runzero
	SEE ALSO
		mem,wmem
	AUTHORS
		Alexey Fomin
endef
