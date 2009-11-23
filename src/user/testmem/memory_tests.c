/**
 * \file memory_tests.c
 * \date Jul 29, 2009
 * \author afomin
 */
#include "common.h"
#include "string.h"

typedef unsigned char datum;
// As much as needed to save mem in memory_test_data_bus
#define MEM_BUF_SIZE 100

inline static print_error(volatile WORD *addr, volatile WORD expected_value) {
	TRACE("FAILED! at addr 0x%08x value 0x%08x (0x%8x expected)\n", addr, *addr,
			expected_value);
}

WORD memory_test_data_bus(volatile WORD *address)
{
    WORD pattern;


    /*
     * Perform a walking 1's test at the given address.
     */
    for (pattern = 1; pattern != 0; pattern <<= 1)
    {
        /*
         * Write the test pattern.
         */
        *address = pattern;

        /*
         * Read it back (immediately is okay for this test).
         */
        if (*address != pattern)
        {
            return (pattern);
        }
    }

    return (0);

}   /* memory_test_data_bus */

WORD *memory_test_addr_bus(WORD * baseAddress, unsigned long nBytes) {
	unsigned long addressMask = (nBytes / sizeof(WORD) - 1);
	unsigned char mem_buf[100];
	unsigned long offset;
	unsigned long testOffset;

	WORD pattern = (datum) 0xAAAAAAAA;
	WORD antipattern = (datum) 0x55555555;

	/*
	 * Write the default pattern at each of the power-of-two offsets.
	 */
	for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
		baseAddress[offset] = pattern;
	}

	/*
	 * Check for address bits stuck high.
	 */
	testOffset = 0;
	baseAddress[testOffset] = antipattern;

	for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
		if (baseAddress[offset] != pattern) {
			return ((WORD *) &baseAddress[offset]);
		}
	}

	baseAddress[testOffset] = pattern;

	/*
	 * Check for address bits stuck low or shorted.
	 */
	for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1) {
		baseAddress[testOffset] = antipattern;

		if (baseAddress[0] != pattern) {
			return ((WORD *) &baseAddress[testOffset]);
		}

		for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
			if ((baseAddress[offset] != pattern) && (offset != testOffset)) {
				return ((WORD *) &baseAddress[testOffset]);
			}
		}

		baseAddress[testOffset] = pattern;
	}

	return (NULL);

}

void memory_test_quick(WORD *base_addr, long int amount) {
	if (memory_test_data_bus(base_addr) == NULL)
		TRACE ("Data bus test ok\n");
	//if (memory_test_addr_bus((WORD *)0x40000000, 0x100000) == NULL)
	if (memory_test_addr_bus(base_addr, amount) == NULL)
		TRACE("Addr bus test ok\n");
	else
		TRACE("Addr bus failed\n");

}

void memory_test_run1(WORD *base_addr, long int amount) {
	WORD *addr, *end_addr;
	volatile WORD value;
	base_addr = (WORD *) ((WORD) base_addr & 0xFFFFFFFC);
	end_addr = base_addr + amount;

	value = 0x1;
	while (value != 0) {
		// Writing
		for (addr = base_addr; addr < end_addr; addr++) {
			*addr = value;
		}
		// Checking
		for (addr = base_addr; addr < end_addr; addr++) {
			if (*addr != value) {
				print_error(addr, value);
				return;
			}
		}
		value <<= 1;
	}
	return;
}

void memory_test_run0(WORD *base_addr, long int amount) {
	WORD *addr, *end_addr;
	volatile WORD value;
	base_addr = (WORD *) ((WORD) base_addr & 0xFFFFFFFC);
	end_addr = base_addr + amount;

	value = 0x1;
	while (value != 0) {
		// Writing
		for (addr = base_addr; addr < end_addr; addr++) {
			*addr = ~value;
		}
		// Checking
		for (addr = base_addr; addr < end_addr; addr++) {
			if (*addr != ~value) {
				print_error(addr, ~value);
				return;
			}
		}
		value <<= 1;
	}
}

void memory_test_address(WORD *base_addr, long int amount) {
	volatile WORD *addr; // address === value in this case. So it must be volatile
	WORD *end_addr;
	base_addr = (WORD *) ((WORD) base_addr & 0xFFFFFFFC);

	long i;

	// Writing
	addr = base_addr;
	end_addr = base_addr + amount;
	// TODO debug:
	TRACE("Starting testing memory from address 0x%08x till 0x%08x:\n", addr, end_addr);
	while (addr < end_addr) {
		if ((WORD) addr % 15 == 0) {
			TRACE("Writing address 0x%08x\n", addr);
		}
		*addr = (WORD) addr;
		addr++;
	}
	// checking
	addr = base_addr;
	end_addr = base_addr + amount;
	addr = base_addr;
	while (addr < end_addr) {
		if ((WORD) addr % 15 == 0) {
			TRACE("Checking address 0x%8x\n", addr);
		}
		if (*addr != (WORD) addr) {
			print_error(addr, (WORD) addr);
			return;
		}
		addr++;
	}
}

void memory_test_chess(WORD *base_addr, long int amount) {
	WORD *addr, *end_addr;
	volatile WORD value;

	base_addr = (WORD *) ((WORD) base_addr & 0xFFFFFFFC);
	end_addr = base_addr + amount;

	// Writing
	value = 0x55555555; // setting first value
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		*addr = value;
	}
	// Testing
	value = 0x55555555; // setting first value
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		if (*addr != value) {
			print_error(addr, ~value);
			return;
		}
	}
	// Writing
	value = ~0x55555555; // setting first value
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		*addr = value;
	}
	// Testing
	value = ~0x55555555; // setting first value
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		if (*addr != value) {
			print_error(addr, ~value);
			return;
		}
	}

}

void memory_test_loop(WORD *addr, long int counter) {
	addr = (WORD *) ((WORD) addr & 0xFFFFFFFC);
	volatile WORD value = 0x55555555;

	// Infinite loop case
	if (counter == 0) {
		while (TRUE) {
			*addr = value;
			if (*addr != value) {
				print_error(addr, value);
				return;
			}
			value = ~value;
		}
	}

	// // Finite loop case
	while (counter--) {
		TRACE("%d=n", counter);
		*addr = value;
		if (*addr != value) {
			print_error(addr, value);
			return;
		}
		value = ~value;
	}
}

