/*
 * ipaux.c
 *
 *  Created on: Mar 20, 2009
 *      Author: anton
 */

#define LITTLE 0// big engian only
unsigned short ptclbsum(unsigned char *addr, int len) {
	unsigned long losum, hisum, mdsum, x;
	unsigned long t1, t2;

	losum = 0;
	hisum = 0;
	mdsum = 0;

	x = 0;
	//if start from not odd addr
	if ((unsigned long) addr & 1) {
		if (len) {
			hisum += addr[0];
			len--;
			addr++;
		}
		x = 1;
	}
	//calc summ by 16 byte
	while (len >= 16) {
		t1 = *(unsigned short*) (addr + 0);
		t2 = *(unsigned short*) (addr + 2);
		mdsum += t1;
		t1 = *(unsigned short*) (addr + 4);
		mdsum += t2;
		t2 = *(unsigned short*) (addr + 6);
		mdsum += t1;
		t1 = *(unsigned short*) (addr + 8);
		mdsum += t2;
		t2 = *(unsigned short*) (addr + 10);
		mdsum += t1;
		t1 = *(unsigned short*) (addr + 12);
		mdsum += t2;
		t2 = *(unsigned short*) (addr + 14);
		mdsum += t1;
		mdsum += t2;
		len -= 16;
		addr += 16;
	}

	while (len >= 2) {
		mdsum += *(unsigned short*) addr;
		len -= 2;
		addr += 2;
	}
	if (x) {
		if (len)
			losum += addr[0];
		if (LITTLE)
			losum += mdsum;
		else
			hisum += mdsum;
	} else {
		if (len)
			hisum += addr[0];
		if (LITTLE)
			hisum += mdsum;
		else
			losum += mdsum;
	}

	losum += hisum >> 8;
	losum += (hisum & 0xff) << 8;
	while (hisum = losum >> 16)
		losum = hisum + (losum & 0xffff);

	return losum & 0xffff;
}
