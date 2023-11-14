/**
 * @file
 *
 * @data 08.02.2016
 * @author: Anton Bondarev
 */
#include <util/log.h>

#include <hal/reg.h>
#include "emac.h"

#include <drivers/common/memory.h>

#include <framework/mod/options.h>

#define MDIO_BASE	OPTION_GET(NUMBER, mdio_base)

#define MAX_PAGESEL 31
#define MAX_IR 20
#define MAX_CR 19
#define MAX_GMIICR 18
#define MAX_PTPCR1 16
#define MAX_DISSPI_SETPG2 0x12
#define MAX_DISSPI_SETPG1 0x11
#define MAX_DISSPI_SETPG0 0x10

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
	int tmp, val, cntr = 0;

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
	val = ADVERTISE_100FULL | ADVERTISE_100HALF |
		ADVERTISE_10HALF | ADVERTISE_10FULL;
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

	tmp = emac_mdio_readreg(MII_LPA);
	tmp &= emac_mdio_readreg(MII_ADVERTISE);

	if (tmp & ADVERTISE_100FULL) {
		log_info("\t100 Mbps FULL");
		emac_set_macctrl(1 | 0x20 | (1 << 15));
	} else if (tmp & ADVERTISE_100HALF) {
		log_info("\t100 Mbps HALF");
		emac_set_macctrl(0 | 0x20 | (1 << 15));
	} else if (tmp & ADVERTISE_10FULL) {
		log_info("\t10 Mbps FULL");
		emac_set_macctrl(1 | 0x20);
	} else if (tmp & ADVERTISE_10HALF) {
		log_info("\t10 Mbps HALF");
		emac_set_macctrl(0 | 0x20);
	} else {
		log_info("Auto negotiatie error");
	}
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

PERIPH_MEMORY_DEFINE(emac_mdio_region, MDIO_BASE, 0x800);
