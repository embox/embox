/**
 * @file
 * @brief arch init
 *
 * @date 21.06.2010
 * @author Anton Kozlov
 */

#include <hal/reg.h>

#define PMC_BASE 0xfffffc00

#define pmc_makereg(offset) (PMC_BASE + offset)

#define CKGR_MOR    pmc_makereg(0x20)
#define CKGR_MCFR   pmc_makereg(0x24)
#define CKGR_PLLR   pmc_makereg(0x2c)
#define PMC_MCKR    pmc_makereg(0x30)
#define PMC_SR      pmc_makereg(0x68)

void arch_init(void) {
	//XXX to disable watchdog

#if 1 //it appeared in trying getting interrupts
	REG_STORE(CKGR_MOR,0x00000801);
	//set moscen (oscount?)
	//poll moscs
	while (!(REG_LOAD(PMC_SR) & 0x01));
	//
	//PLLR:
	//out = in * (mul + 1)
	//after pllr write poll LOCK in PMC_SR
	REG_STORE(CKGR_PLLR,0x00040805); //0_o
	//
	//master clock in PMC_MCKR
	//poll Mwrite_registerCKRDY
	while (!(REG_LOAD(PMC_SR) & 0x08));
	//(
	//for CSS with PLL
	//pres = ...
	//poll MCKRDY
	//css = ...
	//poll MCKRDY
	//;
	//for CSS with main clock
	//
#endif
}

void arch_idle(void) {
}

void arch_shutdown(void) {
	while(1);
}
