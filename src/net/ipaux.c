/**
 * @file ipaux.c
 *
 * @date 20.03.2009
 * @author Anton
 */

#define LITTLE 0// big endian only

unsigned short ptclbsum(unsigned char *addr, int len) {

	register long sum;
	unsigned char oddbyte;
	unsigned short *ptr = (unsigned short *) addr;

	sum = 0;
	while (len > 1) {
		sum += *ptr++;
		len -= 2;
	}

	if (len == 1) {
		oddbyte = 0;
		*((unsigned char *) &oddbyte) = *(unsigned char *) ptr;
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	return sum & 0xffff;

/*
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
	//losum += 0x37;
	return losum & 0xffff;
	*/
}

/**
 * Name  : CRC-16 CCITT
 * Poly  : 0x1021	x^16 + x^12 + x^5 + 1
 * Init  : 0xFFFF
 * Revert: false
 * XorOut: 0x0000
 * Check : 0x29B1 ("123456789")
 * MaxLen: 4095 byte (32767 bit)
 */
unsigned short Crc16( unsigned char *pcBlock, unsigned short len ) {
        unsigned short crc = 0xFFFF;
	unsigned char i;

	while( len-- ) {
    		crc ^= *pcBlock++ << 8;

    		for( i = 0; i < 8; i++ ) {
    			crc = crc & 0x8000 ? ( crc << 1 ) ^ 0x1021 : crc << 1;
    		}
        }
	return crc;
}
