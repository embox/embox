/**
 * @file
 *
 * @data 08.02.2016
 * @author: Anton Bondarev
 */
#include <hal/reg.h>
#include "emac.h"
#include <util/log.h>

#include <framework/mod/options.h>

#define MAX_PAGESEL 31
#define MAX_IR 20
#define MAX_CR 19
#define MAX_GMIICR 18
#define MAX_PTPCR1 16
#define MAX_DISSPI_SETPG2 0x12
#define MAX_DISSPI_SETPG1 0x11
#define MAX_DISSPI_SETPG0 0x10

#define MII_BMCR	    0x00	/* Basic mode control register */
#define MII_BMSR	    0x01	/* Basic mode status register  */
#define MII_PHYSID1	    0x02	/* PHYS ID 1		       */
#define MII_PHYSID2	    0x03	/* PHYS ID 2		       */
#define MII_ADVERTISE	    0x04	/* Advertisement control reg   */
#define MII_LPA		    0x05	/* Link partner ability reg    */
#define MII_EXPANSION	    0x06	/* Expansion register	       */
#define MII_CTRL1000	    0x09	/* 1000BASE-T control	       */
#define MII_STAT1000	    0x0a	/* 1000BASE-T status	       */
#define MII_ESTATUS	    0x0f	/* Extended Status */
#define MII_DCOUNTER	    0x12	/* Disconnect counter	       */
#define MII_FCSCOUNTER	    0x13	/* False carrier counter       */
#define MII_NWAYTEST	    0x14	/* N-way auto-neg test reg     */
#define MII_RERRCOUNTER     0x15	/* Receive error counter       */
#define MII_SREVISION	    0x16	/* Silicon revision	       */
#define MII_RESV1	    0x17	/* Reserved...		       */
#define MII_LBRERROR	    0x18	/* Lpback, rx, bypass error    */
#define MII_PHYADDR	    0x19	/* PHY address		       */
#define MII_RESV2	    0x1a	/* Reserved...		       */
#define MII_TPISTATUS	    0x1b	/* TPI status for 10mbps       */
#define MII_NCONFIG	    0x1c	/* Network interface config    */

/* Basic mode status register. */
#define BMSR_ERCAP		0x0001	/* Ext-reg capability	       */
#define BMSR_JCD		0x0002	/* Jabber detected	       */
#define BMSR_LSTATUS		0x0004	/* Link status		       */
#define BMSR_ANEGCAPABLE	0x0008	/* Able to do auto-negotiation */
#define BMSR_RFAULT		0x0010	/* Remote fault detected       */
#define BMSR_ANEGCOMPLETE	0x0020	/* Auto-negotiation complete   */
#define BMSR_RESV		0x00c0	/* Unused...		       */
#define BMSR_ESTATEN		0x0100	/* Extended Status in R15 */
#define BMSR_100HALF2		0x0200	/* Can do 100BASE-T2 HDX */
#define BMSR_100FULL2		0x0400	/* Can do 100BASE-T2 FDX */
#define BMSR_10HALF		0x0800	/* Can do 10mbps, half-duplex  */
#define BMSR_10FULL		0x1000	/* Can do 10mbps, full-duplex  */
#define BMSR_100HALF		0x2000	/* Can do 100mbps, half-duplex */
#define BMSR_100FULL		0x4000	/* Can do 100mbps, full-duplex */
#define BMSR_100BASE4		0x8000	/* Can do 100mbps, 4k packets  */

/* Advertisement control register. */
#define ADVERTISE_SLCT		0x001f	/* Selector bits	       */
#define ADVERTISE_CSMA		0x0001	/* Only selector supported     */
#define ADVERTISE_10HALF	0x0020	/* Try for 10mbps half-duplex  */
#define ADVERTISE_1000XFULL	0x0020	/* Try for 1000BASE-X full-duplex */
#define ADVERTISE_10FULL	0x0040	/* Try for 10mbps full-duplex  */
#define ADVERTISE_1000XHALF	0x0040	/* Try for 1000BASE-X half-duplex */
#define ADVERTISE_100HALF	0x0080	/* Try for 100mbps half-duplex */
#define ADVERTISE_1000XPAUSE	0x0080	/* Try for 1000BASE-X pause    */
#define ADVERTISE_100FULL	0x0100	/* Try for 100mbps full-duplex */
#define ADVERTISE_1000XPSE_ASYM 0x0100	/* Try for 1000BASE-X asym pause */
#define ADVERTISE_100BASE4	0x0200	/* Try for 100mbps 4k packets  */
#define ADVERTISE_PAUSE_CAP	0x0400	/* Try for pause	       */
#define ADVERTISE_PAUSE_ASYM	0x0800	/* Try for asymetric pause     */
#define ADVERTISE_RESV		0x1000	/* Unused...		       */
#define ADVERTISE_RFAULT	0x2000	/* Say we can detect faults    */
#define ADVERTISE_LPACK		0x4000	/* Ack link partners response  */
#define ADVERTISE_NPAGE		0x8000	/* Next page bit	       */

#define ADVERTISE_FULL (ADVERTISE_100FULL | ADVERTISE_10FULL | \
			ADVERTISE_CSMA)
#define ADVERTISE_ALL (ADVERTISE_10HALF | ADVERTISE_10FULL | \
		       ADVERTISE_100HALF | ADVERTISE_100FULL)


static int phy_num = -1;

void emac_mdio_enable(void) {
	REG_STORE(MDIO_BASE + MDIO_R_CONTROL,
			MDIO_CONTROL_ENABLE | 0x20 /* Freq */);

	while (REG_LOAD(MDIO_BASE + MDIO_R_CONTROL) & MDIO_CONTROL_IDLE);
}

void emac_mdio_writereg(unsigned char reg_num,unsigned short data) {
	unsigned int tmp=0,tmp1=0;

	tmp=(1<<31);
	log_debug("EMW 1 tmp=%x",tmp);
	tmp1=1<<30;
	log_debug("EMW 2 tmp1=%x",tmp1);
	tmp|=tmp1;
	tmp1=((reg_num&0x1f)<<21);
	log_debug("EMW 3 tmp1=%x",tmp1);
	tmp|=tmp1;
	tmp1=((phy_num & 0x1f) << 16);
	log_debug("EMW 4 tmp1=%x",tmp1);
	tmp|=tmp1;
	tmp1=data&0xffff;
	log_debug("EMW 5 tmp1=%x",tmp1);
	tmp|=tmp1;
	log_debug("EMW tmp=%x",tmp);
		log_debug("trace 1");
	REG_STORE(MDIO_BASE+MDIO_R_USERACCESS0,tmp);
		log_debug("trace 2");
	tmp1=0;
	while(REG_LOAD(MDIO_BASE+MDIO_R_USERINTRAW)==0)
	{
		tmp1++;
		log_debug("wait..");
	}
	REG_STORE(MDIO_BASE+MDIO_R_USERINTRAW,REG_LOAD(MDIO_BASE+MDIO_R_USERINTRAW));
	//res=REG_LOAD(MDIO_BASE+MDIO_R_USERACCESS0);
}

int emac_mdio_readreg(unsigned char reg_num) {
	unsigned int res=0;
	unsigned int tmp=0,tmp1=0;

	tmp=(1<<31);
	log_debug("EMR 1 tmp=%x",tmp);
	tmp1=((reg_num&0x1f)<<21);
	log_debug("EMR 2 tmp1=%x",tmp1);
	tmp|=tmp1;
	tmp1=((phy_num & 0x1f) << 16);
	log_debug("EMR 3 tmp1=%x",tmp1);
	tmp|=tmp1;
	log_debug("EMR tmp=%x",tmp);
	REG_STORE(MDIO_BASE+MDIO_R_USERACCESS0,tmp);
	tmp1=0;
	while(REG_LOAD(MDIO_BASE+MDIO_R_USERINTRAW)==0)
	{
		log_debug("wait..");
		tmp1++;
	}
	REG_STORE(MDIO_BASE+MDIO_R_USERINTRAW,REG_LOAD(MDIO_BASE+MDIO_R_USERINTRAW));

	res=REG_LOAD(MDIO_BASE+MDIO_R_USERACCESS0);
	if(((res>>29)&0x1)==0)
	{
		log_error("emac_mdio_readreg err - ACC not - res=%x",res);
		return 0;
	}
	return res&0xffff;
}

void emac_mdelay(int value) {
	volatile int delay = value * 4096;

	while (delay --);
}

void emac_detect_phy(void) {
	uint32_t tmp;
	tmp = REG_LOAD(MDIO_BASE + MDIO_R_ALIVE);

	for (int i = 0; i < 31; i++) {
		if (tmp & (1 << i)) {
			phy_num = i;
			break;
		}
	}

	if (phy_num == -1) {
		log_error("Could not detect PHY");
	}

	log_debug("Detected PHY %d", phy_num);

	REG_STORE(MDIO_BASE+MDIO_R_USERPHYSEL0,phy_num);
}

void emac_autonegotiate(void) {
	int tmp, val, cntr;

	tmp = emac_mdio_readreg(MII_BMCR);
	val = tmp | BMCR_FULLDPLX | BMCR_ANENABLE |
						BMCR_SPEED100;
	log_debug("MII_BMCR=%08x,write%08x", tmp, val);
	emac_mdio_writereg(MII_BMCR, val);

	val = emac_mdio_readreg(MII_ADVERTISE);

	log_debug("MII_ADVERTISE=%08x", val);
#if EMAC_VERSION == 0
	val = PHY_ADV;
#else
	val = ADVERTISE_10FULL;
#endif
	emac_mdio_writereg(MII_ADVERTISE, val);

	tmp = emac_mdio_readreg(MII_BMCR);

	/* Restart Auto_negotiation  */
	emac_mdio_writereg(MII_BMCR, tmp | BMCR_ANRESTART);

	/*check AutoNegotiate complete */
	log_debug("MII_BMCR=%08x,write%08x", tmp, val);
	do {
		emac_mdelay(40000);
		if (emac_mdio_readreg(MII_BMSR) & BMSR_ANEGCOMPLETE)
			break;

		cntr++;
	} while (cntr < 200);

	if (!(emac_mdio_readreg(MII_BMSR) & BMSR_ANEGCOMPLETE)) {
		log_error("Autonegotiation not completed");
	}

	log_debug("MII_BMSR=%08x", tmp);
	log_debug("speed reg = %08x", emac_mdio_readreg(0x1));
}

void emac_mdio_config(void) {
	log_debug("emac_mdio_config started");

	emac_mdio_enable();

	emac_mdelay(10000);

	emac_detect_phy();

	emac_autonegotiate();

#if (OPTION_GET(NUMBER, speed) == 100)
	return;
#else
	emac_mdio_writereg(MAX_CR,res);
	res=emac_mdio_readreg(MAX_GMIICR);
	log_debug("EMC res3=%x",res);
	res|=0x8480;
	log_debug("EMC res4=%x",res);
	emac_mdio_writereg(MAX_GMIICR,res);
	log_debug("emac_mdio_config ended");
#endif
}
