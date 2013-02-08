/**
 * @file
 * @brief
 *
 * @date 13.02.10
 * @author Eldar Abusalimov
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_DIAG_H_
#define DRIVERS_DIAG_H_

extern void diag_init(void);
extern char diag_getc(void);
extern void diag_putc(char ch);
extern int diag_kbhit(void);

#endif /* DRIVERS_DIAG_H_ */
