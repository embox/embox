/**
 * @file
 *
 * @date Feb 19, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_DIAG_NO_DIAG_H_
#define SRC_DRIVERS_DIAG_NO_DIAG_H_

/**
 * @brief Initialize diag input/output
 *
 * @return 0
 */
static inline int diag_init(void) {
	return 0;
}

/**
 * @brief Put char onto diag output.
 *
 * @param ch
 */
static inline void diag_putc(char ch) {
}

/**
 * @brief Get symbol. Can take unpredictable time awaiting input.
 *
 * @return Valid input character
 */
static inline char diag_getc(void) {
	return 0;
}

/**
 * @brief Test diag input buffer.
 *
 * @return 1 if there is a symbol for getc() 0 in other cases
 */
static inline int diag_kbhit(void) {
	return 0;
}

#endif /* SRC_DRIVERS_DIAG_NO_DIAG_H_ */
