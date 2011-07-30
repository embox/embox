/**
 *  @file mac.c
 *  @brief show physical address net device
 *  @date 01.07.11
 *  @author: Gleb Efimov
 *  @author: Ilia Vaprol
 */

#include <types.h>
#include <stdio.h>
#include <net/netdevice.h>
#include <embox/cmd.h>
#include <asm/io.h>

EMBOX_CMD(exec);

#define NE_CMD	 	0x00
#define NE_DATAPORT	0x10	/* NatSemi-defined port window offset. */
#define E8390_STOP	0x01	/* Stop and reset the chip */
#define E8390_START	0x02	/* Start the chip, clear reset */
#define E8390_TRANS	0x04	/* Transmit a frame */
#define E8390_RREAD	0x08	/* Remote read */
#define E8390_RWRITE	0x10	/* Remote write  */
#define E8390_NODMA	0x20	/* Remote DMA */
#define E8390_PAGE0	0x00	/* Select page chip registers */
#define E8390_PAGE1	0x40	/* using the two high-order bits */
#define E8390_PAGE2	0x80	/* Page 3 is invalid. */
#define E8390_CMD	0x00	/* The command register (for all pages) */
/* Page 0 register offsets. */
#define EN0_CLDALO	0x01	/* Low byte of current local dma addr  RD */
#define EN0_STARTPG	0x01	/* Starting page of ring bfr WR */
#define EN0_CLDAHI	0x02	/* High byte of current local dma addr  RD */
#define EN0_STOPPG	0x02	/* Ending page +1 of ring bfr WR */
#define EN0_BOUNDARY	0x03	/* Boundary page of ring bfr RD WR */
#define EN0_TSR		0x04	/* Transmit status reg RD */
#define EN0_TPSR	0x04	/* Transmit starting page WR */
#define EN0_NCR		0x05	/* Number of collision reg RD */
#define EN0_TCNTLO	0x05	/* Low  byte of tx byte count WR */
#define EN0_FIFO	0x06	/* FIFO RD */
#define EN0_TCNTHI	0x06	/* High byte of tx byte count WR */
#define EN0_ISR		0x07	/* Interrupt status reg RD WR */
#define EN0_CRDALO	0x08	/* low byte of current remote dma address RD */
#define EN0_RSARLO	0x08	/* Remote start address reg 0 */
#define EN0_CRDAHI	0x09	/* high byte, current remote dma address RD */
#define EN0_RSARHI	0x09	/* Remote start address reg 1 */
#define EN0_RCNTLO	0x0a	/* Remote byte count reg WR */
#define EN0_RCNTHI	0x0b	/* Remote byte count reg WR */
#define EN0_RSR		0x0c	/* rx status reg RD */
#define EN0_RXCR	0x0c	/* RX configuration reg WR */
#define EN0_TXCR	0x0d	/* TX configuration reg WR */
#define EN0_COUNTER0	0x0d	/* Rcv alignment error counter RD */
#define EN0_DCFG	0x0e	/* Data configuration reg WR */
#define EN0_COUNTER1	0x0e	/* Rcv CRC error counter RD */
#define EN0_IMR		0x0f	/* Interrupt mask reg WR */
#define EN0_COUNTER2	0x0f	/* Rcv missed frame error counter RD */

#define STATION_ADDRESS_SZ	32

static inline void outsw(unsigned short port, const void * addr, unsigned long count) \
{ __asm__ __volatile__ ("cld ; rep ; outs" "w" \
: "=S" (addr), "=c" (count) : "d" (port),"0" (addr),"1" (count)); }

static inline void __outb_p(unsigned char value, unsigned short port)
{
	__asm__ __volatile__ ("out" "b" " %" "b" "0,%" "w" "1" : : "a" (value), "d" (port));
	__asm__ __volatile__ ("outb %al,$0x80");
}
static inline void __outbc_p(unsigned char value, unsigned short port)
{
	__asm__ __volatile__ ("out" "b" " %" "b" "0,%" "" "1" : : "a" (value), "i" (port));
	__asm__ __volatile__ ("outb %al,$0x80");
}

#undef outb_p
#define outb_p(val,port) \
((__builtin_constant_p((port)) && (port) < 256) ? \
	__outbc_p((val),(port)) : \
	__outb_p((val),(port)))

static void test_send(void) {
    unsigned char buf[] = {// ethernet hdr
    		               0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                           0x6C, 0xF0, 0x49, 0x90, 0x24, 0x5B,
                           0x08, 0x06,
                           // arp
                           0x00, 0x01,
                           0x08, 0x00,
                           0x06,
                           0x04,
                           0x00, 0x01,
                           0x6C, 0xF0, 0x49, 0x90, 0x24, 0x5B,
                           0x0A, 0x00, 0x02, 0xDA,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x0A, 0x00, 0x02, 0x36,
                           0x00, 0x00, 0x00, 0x00, // cos min packet has 46 bytes
                           // checksum
                           0xF9, 0xE7, 0x54, 0x4A
    };
    unsigned short count = sizeof buf;
    extern unsigned char tx_start_page;
    extern unsigned long ba;
    long nic_base = ba;

    printf("transmission");
    /* We should already be in page 0, but to be safe... */
    outb_p(E8390_PAGE0+E8390_START+E8390_NODMA, nic_base + NE_CMD);

    /* Now the normal output. */
    outb_p(count & 0xff, nic_base + EN0_RCNTLO);
    outb_p(count >> 8,   nic_base + EN0_RCNTHI);
    outb_p(0x00, nic_base + EN0_RSARLO);
    outb_p(tx_start_page, nic_base + EN0_RSARHI);
    printf(" %X", (int)inb(nic_base + NE_CMD));

    outb_p(E8390_RWRITE + E8390_START, nic_base + NE_CMD);
    printf(" %X", (int)inb(nic_base + NE_CMD));

    outsw(nic_base + NE_DATAPORT, buf, count>>1);
    printf(" %X", (int)inb(nic_base + NE_CMD));
    outb_p(30, nic_base + NE_CMD);
    printf(" %X", (int)inb(nic_base + NE_CMD));
    printf(" complete\n");
}

static int exec(int argv, char **argc) {
	uint8_t i;
	struct net_device *dev;

	dev = netdev_get_by_name("eth0");
	if (NULL == dev) {
		printf("Couldn't find NE2K_PCI device");
		return 1;
	}
	/* Get MAC-Address */
	printf("The current MAC stations address is ");
	for (i = 0; i < dev->addr_len - 1; i++) {
		printf("%2X:", dev->dev_addr[i]);
	}
	printf("%2X.\n", dev->dev_addr[i]);
	test_send();
	return 0;
}

