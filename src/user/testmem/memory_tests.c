/**
 * \file memory_tests.c
 * \date Jul 29, 2009
 * \author afomin
 * \details
 */
inline static print_error(WORD *addr, WORD expected_value) {
	TRACE("FAILED! at addr 0x%8x value 0x%8x (0x%8x expected)\n",
			addr, *addr, expected_value);
}
void memory_test_run0(WORD *base_addr, long int amount) {
	WORD *addr, end_addr = base_addr + amount;
	int i, j;
	WORD value;

	value = 0x1;
	while (non_zero_value != 0) {
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

void memory_test_run1(WORD *base_addr, long int amount) {
	WORD *addr, end_addr = base_addr + amount;
	WORD value;

	value = 0x1;
	while (non_zero_value != 0) {
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
	return;
}

void memory_test_loop(WORD *base_addr, long int counter) {
	WORD value = 0x55555555;
	WORD *addr, *end_addr = base_addr + amount;

	// Infinite loop case
	if (amount == 0) {
		while (TRUE) {
			for (addr = base_addr; addr < end_addr; addr++) {
				*addr = value;
			}
			for (addr = base_addr; addr < end_addr; addr++) {
				if (*addr != value) {
					print_error(addr, value);
					return;
				}
			}
			value = ~value;
		}
	}

	// // Finite loop case
	while (counter--) {
		void memory_test_run1(WORD *base_addr, long int amount) {
			WORD *addr, end_addr = base_addr + amount;
			int i, j;
			WORD value;

			value = 0x1;
			while (non_zero_value != 0) {
				// Writing
				for (addr = base_addr; addr < end_addr; addr++) {
					*addr = value;
				}
				// Checking
				for (addr = base_addr; addr < end_addr; addr++) {
					if (*addr != value) {
						TRACE("FAILED!\n");
						return;
					}
				}
				value <<= 1;
			}
			return;
		}
	}
}

