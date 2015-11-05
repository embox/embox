#include "wdt.h"

void wdt_set_cycle(int period)
{
	WDT_OFF;
	WDT_SET_CNT_EN;
	WDT_SET_CNT(0x000FFFFF);
	WDT_ON;
}
