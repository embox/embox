/**
 * @file
 * @brief
 *
 * @date 28.12.2012
 * @author Anton Bulychev
 */

#include <types.h>

#include <asm/io.h>

#include <module/embox/driver/interrupt/lapic.h>

void lapic_timer_init(uint32_t quantum) {
	uint32_t tmp, cpubusfreq;

	//map APIC timer to an interrupt, and by that enable it in one-shot mode
	lapic_write(LAPIC_LVT_TR, 32);
	//set up divide value to 16
	lapic_write(LAPIC_TIMER_DCR, 0x03);

	//initialize PIT Ch 2 in one-shot mode
	//waiting 1 sec could slow down boot time considerably,
	//so we'll wait 1/100 sec, and multiply the counted ticks
	outb(0x61,(inb(0x61) & 0xFD)|1);
	outb(0x43,0xB2);
	//1193180/100 Hz = 11931 = 2e9bh
	outb(0x42,0x9B);	//LSB
	inb(0x60);	//short delay
	outb(0x42,0x2E);	//MSB

	//reset PIT one-shot counter (start counting)
	tmp = inb(0x61) & 0xFE;
	outb(0x61,(uint8_t)tmp);		//gate low
	outb(0x61,(uint8_t)tmp|1);		//gate high
	//reset APIC timer (set counter to -1)
	lapic_write(LAPIC_TIMER_ICR, 0xFFFFFFFF);

	//now wait until PIT counter reaches zero
	while(!(inb(0x61)&0x20));

	//stop APIC timer
	lapic_write(LAPIC_LVT_TR, 0x10000);

	//now do the math...
	cpubusfreq=((0xFFFFFFFF - lapic_read(LAPIC_TIMER_ICR))+1)*16*100;
	tmp = cpubusfreq / quantum / 16;

	//sanity check, now tmp holds appropriate number of ticks, use it as APIC timer counter initializer
	lapic_write(LAPIC_TIMER_ICR, tmp<16?16:tmp);
	//finally re-enable timer in periodic mode
	lapic_write(LAPIC_LVT_TR, 32 | 0x20000);
	//setting divide value register again not needed by the manuals
	//although I have found buggy hardware that required it
	lapic_write(LAPIC_TIMER_DCR, 0x03);
}
