/*
 * ipaux.c
 *
 *  Created on: Mar 20, 2009
 *      Author: anton
 */

ushort ptclcsum(Block *bp, int offset, int len) {
	uchar *addr;
	ulong losum, hisum;
	ushort csum;
	int odd, blocklen, x;

	/* Correct to front of data area */
	while (bp != nil && offset && offset >= BLEN(bp)) {
		offset -= BLEN(bp);
		bp = bp->next;
	}
	if (bp == nil)
		return 0;

	addr = bp->rp + offset;
	blocklen = BLEN(bp) - offset;

	if (bp->next == nil) {
		if (blocklen < len)
			len = blocklen;
		return ~ptclbsum(addr, len) & 0xffff;
	}

	losum = 0;
	hisum = 0;

	odd = 0;
	while (len) {
		x = blocklen;
		if (len < x)
			x = len;

		csum = ptclbsum(addr, x);
		if (odd)
			hisum += csum;
		else
			losum += csum;
		odd = (odd + x) & 1;
		len -= x;

		bp = bp->next;
		if (bp == nil)
			break;
		blocklen = BLEN(bp);
		addr = bp->rp;
	}

	losum += hisum >> 8;
	losum += (hisum & 0xff) << 8;
	while ((csum = losum >> 16) != 0)
		losum = csum + (losum & 0xffff);

	return ~losum & 0xffff;
}

ushort ptclbsum(uchar *addr, int len) {
	ulong losum, hisum, mdsum, x;
	ulong t1, t2;

	losum = 0;
	hisum = 0;
	mdsum = 0;

	x = 0;
	if ((ulong) addr & 1) {
		if (len) {
			hisum += addr[0];
			len--;
			addr++;
		}
		x = 1;
	}
	while (len >= 16) {
		t1 = *(ushort*) (addr + 0);
		t2 = *(ushort*) (addr + 2);
		mdsum += t1;
		t1 = *(ushort*) (addr + 4);
		mdsum += t2;
		t2 = *(ushort*) (addr + 6);
		mdsum += t1;
		t1 = *(ushort*) (addr + 8);
		mdsum += t2;
		t2 = *(ushort*) (addr + 10);
		mdsum += t1;
		t1 = *(ushort*) (addr + 12);
		mdsum += t2;
		t2 = *(ushort*) (addr + 14);
		mdsum += t1;
		mdsum += t2;
		len -= 16;
		addr += 16;
	}
	while (len >= 2) {
		mdsum += *(ushort*) addr;
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
