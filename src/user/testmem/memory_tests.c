/**
 * @file
 * @date 29.07.2009
 * @author Alexey Fomin
 */
#include <shell_command.h>
#include <string.h>

/* TODO: move to header: the type is used for one width data bus */
typedef uint32_t datum;

inline static void print_error(volatile uint32_t *addr, volatile uint32_t expected_value) {
	TRACE("FAILED! at addr 0x%08x value 0x%08x (0x%8x expected)\n", (unsigned)addr, *addr,
			expected_value);
}

/**
 * Test the data bus wiring by performing
 * walking 1's test at a fixed address
 * which is given as argument.
 * Returns the written value if it doesn't
 * agree with the value which has been read
 * from the address and zero if test
 * is finished correctly.
 */
static uint32_t memory_test_data_bus(volatile uint32_t *address) {
	uint32_t pattern;
	/*
	 * Perform a walking 1's test at the given address.
	 */
	for (pattern = 1; pattern != 0; pattern <<= 1) {
		/*
		 * Write the test pattern.
		 */
		*address = pattern;

		/*
		 * Read it back (immediately is okay for this test).
		 */
		if (*address != pattern) {
			return (pattern);
		}
	}

	return (0);

} /* memory_test_data_bus */

/**
 * The test finds single-bit address failures.
 * Test the address bus wiring in a memory
 * region by performing a walking 1's test
 * on the relevant bits of the address and
 * checking for aliasing.
 * Parameters: the base address, tested region size
 * Returns the failure address if test fails and
 * NULL if test is finished correctly.
 */
/* TODO think about signature: err_t name(..., uint32_t *fault_address) -- Eldar*/
static uint32_t *memory_test_addr_bus(uint32_t * baseAddress, unsigned long nBytes, uint32 template) {
	unsigned long addressMask = (nBytes / sizeof(uint32_t) - 1);
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

/**
 * Run memory_test_data_bus and memory_test_addr_bus
 * for quick checking of memory.
 */
static int memory_test_quick(uint32_t *base_addr, long int amount) {
	if (0 == memory_test_data_bus(base_addr)) {
		TRACE ("Data bus test ok\n");
	} else {
		TRACE("Data bus failed\n");
		return 0;
	}

	if (0 == memory_test_addr_bus(base_addr, amount)) {
		TRACE("Addr bus test ok\n");
	} else {
		TRACE("Addr bus failed\n");
		return 0;
	}
	return 1;
}

/**
 * Test the data bus wiring by performing
 * walking 1's test at a block of memory.
 * Parameters: the base address, tested region size.
 * Returns the address, its value and the value
 * which were written by this address, if the test is
 * failed and one if the test is passed.
 * @param base_addr
 */
static int memory_test_walking_one(uint32_t *base_addr, long int amount) {
	uint32_t *addr, *end_addr;
	volatile uint32_t value;
	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);
	end_addr = base_addr + amount;

	value = 0x1;
	while (value != 0) {
		/* Writing*/
		for (addr = base_addr; addr < end_addr; addr++) {
			*addr = value;
		}
		/* Checking*/
		for (addr = base_addr; addr < end_addr; addr++) {
			if (*addr != value) {
				print_error(addr, value);
				return 0;
			}
		}
		value <<= 1;
	}
	return 1;
}

/**
 * Test the data bus wiring by performing
 * walking 0's test at a block of memory.
 * Parameters: the base address, tested region size.
 * Returns the address, its value and the value
 * which were written by this address, if the test is
 * failed and one if the test is passed.
 */
static int memory_test_walking_zero(uint32_t *base_addr, long int amount) {
	uint32_t *addr, *end_addr;
	volatile uint32_t value;
	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);
	end_addr = base_addr + amount;

	value = 0x1;
	while (value != 0) {
		/* Writing */
		for (addr = base_addr; addr < end_addr; addr++) {
			*addr = ~value;
		}
		/* Checking */
		for (addr = base_addr; addr < end_addr; addr++) {
			if (*addr != ~value) {
				print_error(addr, ~value);
				return 0;
			}
		}
		value <<= 1;
	}
	return 1;
}

static int memory_test_address(uint32_t *base_addr, long int amount) {
	/* address === value in this case. So it must be volatile*/
	volatile uint32_t *addr;
	uint32_t *end_addr;
	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);

	/* Writing */
	addr = base_addr;
	end_addr = base_addr + amount;
	while (addr < end_addr) {
		if ((uint32_t) addr % 15 == 0) {
			TRACE("Writing address 0x%08x\n", (unsigned)addr);
		}
		*addr = (uint32_t) addr;
		addr++;
	}
	/* checking */
	addr = base_addr;
	end_addr = base_addr + amount;
	addr = base_addr;
	while (addr < end_addr) {
		if ((uint32_t) addr % 15 == 0) {
			TRACE("Checking address 0x%8x\n", (unsigned)addr);
		}
		if (*addr != (uint32_t) addr) {
			print_error(addr, (uint32_t) addr);
			return 0;
		}
		addr++;
	}
	return 1;
}

static int memory_test_chess(uint32_t *base_addr, long int amount) {
	uint32_t *addr, *end_addr;
	volatile uint32_t value;

	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);
	end_addr = base_addr + amount;

	/* Writing */
	value = 0x55555555; /* setting first value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		*addr = value;
	}
	/* Testing */
	value = 0x55555555; /* setting second value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		if (*addr != value) {
			print_error(addr, ~value);
			return 0;
		}
	}
	/* Writing */
	value = ~0x55555555; /* setting first value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		*addr = value;
	}
	/* Testing */
	value = ~0x55555555; /* setting second value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		if (*addr != value) {
			print_error(addr, ~value);
			return 0;
		}
	}
	return 1;
}

/**
* This test is needed for the oscilloscope.
*/
static int memory_test_loop(uint32_t *addr, long int counter) {
	volatile uint32_t value = 0x55555555;
	addr = (uint32_t *) ((uint32_t) addr & 0xFFFFFFFC);

	/* Infinite loop case */
	if (counter == 0) {
		while (true) {
			*addr = value;
			if (*addr != value) {
				print_error(addr, value);
				return 0;
			}
			value = ~value;
		}
	}

	/*Finite loop case*/
	while (counter--) {
		TRACE("%ld=n", counter);
		*addr = value;
		if (*addr != value) {
			print_error(addr, value);
			return 0;
		}
		value = ~value;
	}
	return 1;
}
#define DECLARE_MEM_TESTS(...) \
		memory_test_t memtest_array[] ={__ARGS__};

#define MEM_TEST(name, func) \
		{name, memory_test_chess}

/*
memory_test_t memtest_array[] ={
		{"chess", memory_test_chess},
		{"loop", memory_test_loop}
};
*/
DECLARE_MEM_TESTS(
		MEM_TEST("chess", memory_test_chess),
		MEM_TEST("loop", memory_test_loop)
		)
