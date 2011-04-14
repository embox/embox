/**
 * @file
 *
 * @date 29.07.09
 * @author Alexey Fomin
 *         Daria Teplykh
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util/array.h>
#include "memory_tests.h"

static inline void print_error(volatile uint32_t *addr,
		volatile uint32_t expected_value) {
	TRACE("FAILED! at address 0x%08x value 0x%08x (0x%8x expected)\n",
			(unsigned)addr, *addr, expected_value);
}

static inline int return_error(memtest_err_t *s_err, const char *test_name,
		volatile uint32_t *address, uint32_t received_value,
		volatile uint32_t expected_value) {
	print_error(address, expected_value);
	s_err->test_name = test_name;
	s_err->addr = (uint32_t) address;
	s_err->received_value = received_value;
	s_err->expected_value = expected_value;
	return MEMTEST_RETCODE_FAILED;
}

/**
 * Test the data bus wiring by performing walking 1's test at a fixed address
 * which is given as argument.
 * @param address - the base address
 * @param block_size - the size of memory for testing
 * @return -1 in case the test is failed and 0 otherwise
 */
/* TODO: remove to express_tests*/
static int memory_test_data_bus(volatile uint32_t *address, size_t block_size,
		uint32_t template, memtest_err_t *s_err) {
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
			return return_error(s_err, "databus", address, *address, pattern);
		}
	}

	return MEMTEST_RETCODE_PASSED;

} /* memory_test_data_bus */

/**
 * The test finds single-bit address failures. Test the address bus wiring in
 * a memory region by performing a walking 1's test on the relevant bits of
 * the address and checking for aliasing.
 * @param base_addr - the base address
 * @param block_size - the size of the region which will be tested
 * @return -1 in case the test is failed and 0 otherwise
 */
/* TODO: remove to express_tests*/
static int memory_test_addr_bus(uint32_t *base_address,
		unsigned long block_size, uint32_t template, memtest_err_t *s_err) {
	unsigned long addressMask = (block_size / sizeof(uint32_t) - 1);
	unsigned long offset;
	unsigned long testOffset;

	uint32_t pattern = (uint32_t) 0xAAAAAAAA;
	uint32_t antipattern = (uint32_t) template;

	/*
	 * Write the default pattern at each of the power-of-two offsets.
	 */
	for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
		base_address[offset] = pattern;
	}

	/*
	 * Check for address bits stuck high.
	 */
	testOffset = 0;
	base_address[testOffset] = antipattern;

	for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
		if (base_address[offset] != pattern) {
			return return_error(s_err, "addrbus",
					(uint32_t *) &base_address[offset], base_address[offset],
					pattern);
		}
	}

	base_address[testOffset] = pattern;

	/*
	 * Check for address bits stuck low or shorted.
	 */
	for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1) {
		base_address[testOffset] = antipattern;

		if (base_address[0] != pattern) {
			return return_error(s_err, "addrbus",
					(uint32_t *) &base_address[testOffset], base_address[0],
					pattern);
		}

		for (offset = 1; (offset & addressMask) != 0; offset <<= 1) {
			if ((base_address[offset] != pattern) && (offset != testOffset)) {
				return return_error(s_err, "addrbus",
						(uint32_t *) &base_address[testOffset],
						base_address[offset], pattern);
			}
		}

		base_address[testOffset] = pattern;
	}

	return MEMTEST_RETCODE_PASSED;

}

/**
 * Run memory_test_data_bus and memory_test_addr_bus for the quick checking
 * of memory.
 * @param base_addr - the base address
 * @param amount - the size of the region which will be tested
 * @return -1 in case the test is failed and 0 otherwise
 */
/* TODO: remove to express_tests*/
static int memory_test_quick(uint32_t *base_addr, long int amount,
		uint32_t template, memtest_err_t *s_err) {
	if (MEMTEST_RETCODE_PASSED == memory_test_data_bus(base_addr, 1, template,
			s_err)) {
		TRACE ("Data bus test ok\n");
	} else {
		TRACE("Data bus failed\n");
		return MEMTEST_RETCODE_FAILED;
	}

	if (MEMTEST_RETCODE_PASSED == memory_test_addr_bus(base_addr, amount,
			template, s_err)) {
		TRACE("Addr bus test ok\n");
	} else {
		TRACE("Addr bus failed\n");
		return MEMTEST_RETCODE_FAILED;
	}
	return MEMTEST_RETCODE_PASSED;
}

/**
 * Test the data bus wiring by performing walking 1's test at a memory block.
 * @param base_addr - the base address
 * @param amount - the size of the region which will be tested
 * @return -1 in case the test is failed and 0 otherwise
 */
static int memory_test_walking_one(uint32_t *base_addr, long int amount,
		uint32_t template, memtest_err_t *s_err) {
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
				return return_error(s_err, "runone", addr, *addr, value);
			}
		}
		value <<= 1;
	}
	return MEMTEST_RETCODE_PASSED;
}

/**
 * Test the data bus wiring by performing walking 0's test at a memory block.
 * @param base_addr - the base address
 * @param amount - the size of the region which will be tested
 * @return -1 in case the test is failed and 0 otherwise
 */
static int memory_test_walking_zero(uint32_t *base_addr, long int amount,
		uint32_t template, memtest_err_t *s_err) {
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
				return return_error(s_err, "runzero", addr, *addr, ~value);
			}
		}
		value <<= 1;
	}
	return MEMTEST_RETCODE_PASSED;
}

/**
 * In the given block of memory in each memory cell the address of this cell
 * is written. And then the address and its written value are checked.
 * @param base_addr - the base address
 * @param amount - the size of the region which will be tested
 * @return -1 in case the test is failed and 0 otherwise
 */
static int memory_test_address(uint32_t *base_addr, long int amount,
		uint32_t template, memtest_err_t *s_err) {
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
			return return_error(s_err, "address", addr, *addr, (uint32_t) addr);
		}
		addr++;
	}
	return MEMTEST_RETCODE_PASSED;
}

/**
 *
 * @param base_addr - the base address
 * @param amount - the size of the region which will be tested
 * @param template - the value, which is written to the address
 * @return -1 in case the test is failed and 0 otherwise
 */
static int memory_test_chess(uint32_t *base_addr, long int amount,
		uint32_t template, memtest_err_t *s_err) {
	uint32_t *addr, *end_addr;
	volatile uint32_t value;

	base_addr = (uint32_t *) ((uint32_t) base_addr & 0xFFFFFFFC);
	end_addr = base_addr + amount;

	/* Writing */
	value = template; /* setting first value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		*addr = value;
	}
	/* Testing */
	value = template; /* setting second value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		if (*addr != value) {
			return return_error(s_err, "chess", addr, *addr, ~value);
		}
	}
	/* Writing */
	value = ~template; /* setting first value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		*addr = value;
	}
	/* Testing */
	value = ~template; /* setting second value */
	for (addr = base_addr; addr < end_addr; addr++, value = ~value) {
		if (*addr != value) {
			return return_error(s_err, "chess", addr, *addr, ~value);
		}
	}
	return MEMTEST_RETCODE_PASSED;
}

/**
 * This test is needed for the oscilloscope.
 * @return -1 in case the test is failed and 0 otherwise
 */
static int memory_test_loop(uint32_t *addr, long int counter,
		uint32_t template, memtest_err_t *s_err) {
	volatile uint32_t value = template;
	addr = (uint32_t *) ((uint32_t) addr & 0xFFFFFFFC);

	/* Infinite loop case */
	if (counter == 0) {
		while (true) {
			*addr = value;
			if (*addr != value) {
				return return_error(s_err, "loop", addr, *addr, value);
			}
			value = ~value;
		}
	}

	/*Finite loop case*/
	while (counter--) {
		TRACE("%ld=n", counter);
		*addr = value;
		if (*addr != value) {
			return return_error(s_err, "loop", addr, *addr, value);
		}
		value = ~value;
	}
	return MEMTEST_RETCODE_PASSED;
}

/* Add the test name and the function name of the test if the test is added */
static memory_test_t memtest_array[] = {
		{ "runzero", memory_test_walking_zero },
		{ "runone", memory_test_walking_one },
		{ "address", memory_test_address },
		{ "chess", memory_test_chess },
		{ "quick", memory_test_quick },
		{ "loop", memory_test_loop }
};
/**
 * @param test_name the name of the test
 * @return the function of the needed memory test and NULL otherwise
 */
TEST_MEM_FUNC *get_memtest_func(const char *test_name) {
	int i;
	for (i = 0; i < ARRAY_SIZE(memtest_array); i++) {
		if (strcmp(test_name, memtest_array[i].test_name) == 0) {
			return &memtest_array[i].func;
		}
	}
	return NULL;
}
