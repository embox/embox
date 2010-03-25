/**
 * @file
 *
 * @date 29.07.2009
 * @author Alexey Fomin
 * @author Alexander Batyukov
 *         - some changes in interface and designed as express test
 */

#include <string.h>
#include <embox/test.h>

EMBOX_TEST(run);

/* FIXME what does this type mean? -- Eldar*/
typedef unsigned char datum;
/* As much as needed to save mem in memory_test_data_bus */
#define MEM_BUF_SIZE 100

inline static void print_error(volatile uint32_t *addr,
		volatile uint32_t expected_value) {
	TRACE("FAILED! at addr 0x%08x value 0x%08x (0x%8x expected)\n",
			(unsigned)addr, *addr, expected_value);
}

#if 0
static uint32_t memory_test_data_bus(volatile uint32_t *address) {
	uint32_t pattern;

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
} /* memory_test_data_bus */

// TODO think about signature: err_t name(..., uint32_t *fault_address) -- Eldar
static uint32_t *memory_test_addr_bus(uint32_t * baseAddress, unsigned long nBytes) {
	unsigned long addressMask = (nBytes / sizeof(uint32_t) - 1);
	//unsigned char mem_buf[100];
	unsigned long offset;
	unsigned long testOffset;

	uint32_t pattern = (datum) 0xAAAAAAAA;
	uint32_t antipattern = (datum) 0x55555555;

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
			return ((uint32_t *) &baseAddress[offset]);
		}
	}

	baseAddress[testOffset] = pattern;

	/*
	 * Check for address bits stuck low or shorted.
	 */
	for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1) {
		baseAddress[testOffset] = antipattern;

		if (baseAddress[0] != pattern) {
			return ((uint32_t *) &baseAddress[testOffset]);
		}

		for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
			if ((baseAddress[offset] != pattern) && (offset != testOffset)) {
				return ((uint32_t *) &baseAddress[testOffset]);
			}
		}

		baseAddress[testOffset] = pattern;
	}

	return NULL;

}

static int memory_test_quick(uint32_t *base_addr, long int amount) {
	if (0 == memory_test_data_bus(base_addr)) {
		TRACE ("Data bus test ok\n");
	} else {
		TRACE("Data bus failed\n");
		return -1;
	}

	//if (memory_test_addr_bus((uint32_t *)0x40000000, 0x100000) == NULL)
	if (0 == memory_test_addr_bus(base_addr, amount)) {
		TRACE("Addr bus test ok\n");
	} else {
		TRACE("Addr bus failed\n");
		return -1;
	}
	return 0;
}

static int memory_test_run1(uint32_t *base_addr, long int amount) {
	uint32_t *addr, *end_addr;
	volatile uint32_t value;
	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);
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
				return -1;
			}
		}
		value <<= 1;
	}
	return 0;
}
#endif

static int memory_test_run0(uint32_t *base_addr, long int amount) {
	uint32_t *addr, *end_addr;
	volatile uint32_t value;
	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);
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
				return -1;
			}
		}
		value <<= 1;
	}
	return 0;
}

#if 0
static int memory_test_address(uint32_t *base_addr, long int amount) {
	// address === value in this case. So it must be volatile
	volatile uint32_t *addr;
	uint32_t *end_addr;
	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);

	// Writing
	addr = base_addr;
	end_addr = base_addr + amount;
	// TODO debug:
	TRACE("Starting testing memory from address 0x%08x till 0x%08x:\n",
			(unsigned)addr, (unsigned)end_addr);
	while (addr < end_addr) {
		if ((uint32_t) addr % 15 == 0) {
			TRACE("Writing address 0x%08x\n", (unsigned)addr);
		}
		*addr = (uint32_t) addr;
		addr++;
	}
	// checking
	addr = base_addr;
	end_addr = base_addr + amount;
	addr = base_addr;
	while (addr < end_addr) {
		if ((uint32_t) addr % 15 == 0) {
			TRACE("Checking address 0x%8x\n", (unsigned)addr);
		}
		if (*addr != (uint32_t) addr) {
			print_error(addr, (uint32_t) addr);
			return -1;
		}
		addr++;
	}
	return 0;
}
#endif

static int memory_test_chess(uint32_t *base_addr, long int amount) {
	uint32_t *addr, *end_addr;
	volatile uint32_t value;

	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);
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
			return -1;
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
			return -1;
		}
	}
	return 0;
}

#if 0
static int memory_test_loop(uint32_t *addr, long int counter) {
	volatile uint32_t value = 0x55555555;
	addr = (uint32_t *) ((uint32_t) addr & 0xFFFFFFFC);

	// Infinite loop case
	if (counter == 0) {
		while (true) {
			*addr = value;
			if (*addr != value) {
				print_error(addr, value);
				return -1;
			}
			value = ~value;
		}
	}

	// // Finite loop case
	while (counter--) {
		TRACE("%d=n", (unsigned)counter);
		*addr = value;
		if (*addr != value) {
			print_error(addr, value);
			return -1;
		}
		value = ~value;
	}
	return 0;
}
#endif

static int run(void) {
	if (0 != memory_test_run0((uint32_t *) 0x0, 0x10000)) {
		TRACE("memory test run 0 error FAILED\n");
		return -1;
	}
	if (0 != memory_test_run0((uint32_t *) 0x10000, 0x10000)) {
		TRACE("memory test run 1 error FAILED\n");
		return -1;
	}
	if (0 != memory_test_chess((uint32_t *) 0x20000, 0x10000)) {
		TRACE("memory test chess FAILED\n");
		return -1;
	}
	return 0;
}
