/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.09.2012
 */
#include <hal/reg.h>

#define TRISB 0xBF886040
#define PORTB 0xBF886050

void maximite_blink(void) {
	REG_STORE(TRISB, 0);
	REG_STORE(PORTB, 0xffff);
}
