/**
 * @file
 * @brief TODO
 *
 * @date 13.02.10
 * @author Eldar Abusalimov
 */

#ifndef DRIVERS_DIAG_H_
#define DRIVERS_DIAG_H_

extern void diag_init(void);

extern char diag_getc(void);

extern void diag_putc(char ch);

extern int diag_has_symbol(void);

#endif /* DRIVERS_DIAG_H_ */
