/**
 * @file fec_mdio.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.05.2018
 */

#include "fec.h"
#include <hal/reg.h>
#include <net/mii.h>
#include <util/log.h>

#define FEC_IEVENT_HBERR		0x80000000
#define FEC_IEVENT_BABR			0x40000000
#define FEC_IEVENT_BABT			0x20000000
#define FEC_IEVENT_GRA			0x10000000
#define FEC_IEVENT_TXF			0x08000000
#define FEC_IEVENT_TXB			0x04000000
#define FEC_IEVENT_RXF			0x02000000
#define FEC_IEVENT_RXB			0x01000000
#define FEC_IEVENT_MII			0x00800000
#define FEC_IEVENT_EBERR		0x00400000
#define FEC_IEVENT_LC			0x00200000
#define FEC_IEVENT_RL			0x00100000
#define FEC_IEVENT_UN			0x00080000

#define FEC_IMASK_HBERR			0x80000000
#define FEC_IMASK_BABR			0x40000000
#define FEC_IMASKT_BABT			0x20000000
#define FEC_IMASK_GRA			0x10000000
#define FEC_IMASKT_TXF			0x08000000
#define FEC_IMASK_TXB			0x04000000
#define FEC_IMASKT_RXF			0x02000000
#define FEC_IMASK_RXB			0x01000000
#define FEC_IMASK_MII			0x00800000
#define FEC_IMASK_EBERR			0x00400000
#define FEC_IMASK_LC			0x00200000
#define FEC_IMASKT_RL			0x00100000
#define FEC_IMASK_UN			0x00080000

#define FEC_RCNTRL_MAX_FL_SHIFT		16
#define FEC_RCNTRL_LOOP			0x00000001
#define FEC_RCNTRL_DRT			0x00000002
#define FEC_RCNTRL_MII_MODE		0x00000004
#define FEC_RCNTRL_PROM			0x00000008
#define FEC_RCNTRL_BC_REJ		0x00000010
#define FEC_RCNTRL_FCE			0x00000020
#define FEC_RCNTRL_RGMII		0x00000040
#define FEC_RCNTRL_RMII			0x00000100
#define FEC_RCNTRL_RMII_10T		0x00000200

#define FEC_TCNTRL_GTS			0x00000001
#define FEC_TCNTRL_HBC			0x00000002
#define FEC_TCNTRL_FDEN			0x00000004
#define FEC_TCNTRL_TFC_PAUSE		0x00000008
#define FEC_TCNTRL_RFC_PAUSE		0x00000010

#define FEC_ECNTRL_RESET		0x00000001	/* reset the FEC */
#define FEC_ECNTRL_ETHER_EN		0x00000002	/* enable the FEC */
#define FEC_ECNTRL_SPEED		0x00000020
#define FEC_ECNTRL_DBSWAP		0x00000100

#define FEC_X_WMRK_STRFWD		0x00000100

#define FEC_X_DES_ACTIVE_TDAR		0x01000000

#define FEC_R_DES_ACTIVE_RDAR		0x01000000

/* MII-related definitios */
#define FEC_MII_DATA_ST		0x40000000	/* Start of frame delimiter */
#define FEC_MII_DATA_OP_RD	0x20000000	/* Perform a read operation */
#define FEC_MII_DATA_OP_WR	0x10000000	/* Perform a write operation */
#define FEC_MII_DATA_PA_MSK	0x0f800000	/* PHY Address field mask */
#define FEC_MII_DATA_RA_MSK	0x007c0000	/* PHY Register field mask */
#define FEC_MII_DATA_TA		0x00020000	/* Turnaround */
#define FEC_MII_DATA_DATAMSK	0x0000ffff	/* PHY data field */

#define FEC_MII_DATA_RA_SHIFT	18	/* MII Register address bits */
#define FEC_MII_DATA_PA_SHIFT	23	/* MII PHY address bits */

#define FEC_MII_DATA_RA_SHIFT	18	/* MII Register address bits */
#define FEC_MII_DATA_PA_SHIFT	23	/* MII PHY address bits */
int fec_mdio_read(struct fec_priv *fec, uint8_t regAddr) {
	uint32_t reg;		/* convenient holder for the PHY register */
	uint32_t phy;		/* convenient holder for the PHY */
	int val;
	int retry = 0;

	/* Reading from any PHY's register is done by properly
	 * programming the FEC's MII data register. */
	REG32_STORE(ENET_EIR, FEC_IEVENT_MII);
	reg = regAddr << FEC_MII_DATA_RA_SHIFT;
	phy = fec->phy_id << FEC_MII_DATA_PA_SHIFT;

	REG32_STORE(ENET_MMFR, FEC_MII_DATA_ST | FEC_MII_DATA_OP_RD |
			FEC_MII_DATA_TA | phy | reg);

	/* Wait for the related interrupt */
	while (!(REG32_LOAD(ENET_EIR) & FEC_IEVENT_MII)) {
		if (retry++ > 0xffff) {
			log_error("MDIO write failed");
			return -1;
		}
	}

	/* Clear MII interrupt bit */
	REG32_STORE(ENET_EIR, FEC_IEVENT_MII);

	/* It's now safe to read the PHY's register */
	val = REG32_LOAD(ENET_MMFR) & 0xFFFF;
	log_debug("reg:%02x val:%#x", regAddr, val);
	return val;
}

void fec_mii_setspeed(void) {
	/*
	 * Set MII_SPEED = (1/(mii_speed * 2)) * System Clock
	 * and do not drop the Preamble.
	 *
	 * The i.MX28 and i.MX6 types have another field in the MSCR (aka
	 * MII_SPEED) register that defines the MDIO output hold time. Earlier
	 * versions are RAZ there, so just ignore the difference and write the
	 * register always.
	 * The minimal hold time according to IEE802.3 (clause 22) is 10 ns.
	 * HOLDTIME + 1 is the number of clk cycles the fec is holding the
	 * output.
	 * The HOLDTIME bitfield takes values between 0 and 7 (inclusive).
	 * Given that ceil(clkrate / 5000000) <= 64, the calculation for
	 * holdtime cannot result in a value greater than 3.
	 */
	uint32_t pclk = 0xdeadbeef;//imx_get_fecclk();
	uint32_t speed = pclk / 5000000;
	uint32_t hold = (pclk / 100000000) - 1;
	REG32_STORE(ENET_MSCR, speed << 1 | hold << 8);
	log_debug("mii_speed %08x", REG32_LOAD(ENET_MSCR));
}

static int fec_mdio_write(struct fec_priv *fec, uint8_t regAddr, uint16_t data) {
	uint32_t reg;		/* convenient holder for the PHY register */
	uint32_t phy;		/* convenient holder for the PHY */
	int retry = 0;

	reg = regAddr << FEC_MII_DATA_RA_SHIFT;
	phy = fec->phy_id << FEC_MII_DATA_PA_SHIFT;

	REG32_STORE(ENET_MMFR, FEC_MII_DATA_ST | FEC_MII_DATA_OP_WR |
		FEC_MII_DATA_TA | phy | reg | data);

	/* Wait for the MII interrupt */
	while (!(REG32_LOAD(ENET_EIR) & FEC_IEVENT_MII)) {
		if (retry++ > 0xffff) {
			log_error("MDIO write failed");
			return -1;
		}
	}

	/* Clear MII interrupt bit */
	REG32_STORE(ENET_EIR, FEC_IEVENT_MII);
	log_debug("reg:%02x val:%#x", regAddr, data);

	return 0;
}

static int miiphy_wait_aneg(struct fec_priv *fec) {
	int retry = 0;
	int status;

	/* Wait for AN completion */
	do {
		if (retry > 10000) {
			log_error("Autonegotiation timeout");
			return -1;
		}

		int timeout = 0xffffff;
		while(timeout--);

		status = fec_mdio_read(fec, MII_BMSR);
		if (status < 0) {
			log_error("Autonegotiation failed. Status: %d",
					status);
			return -1;
		}
	} while (!(status & BMSR_LSTATUS));

	return 0;
}

static void mdio_detect_phy(struct fec_priv *fec) {
	for (int i = 0; i < 32; i++) {
		fec->phy_id = i;
		if (fec_mdio_read(fec, 0x1) != 0xffff) {
			log_debug("Detected phyid=%d", i);
			break;
		}
	}
}

static void miiphy_reset(struct fec_priv *fec) {
	int retry = 0;
	uint32_t reg;
	log_debug("Reset PHY");
	reg = fec_mdio_read(fec, MII_BMCR);
	fec_mdio_write(fec, MII_BMCR, reg | BMCR_RESET);

	while (fec_mdio_read(fec, MII_BMCR) & BMCR_RESET) {
		int timeout = 0xfffffff;
		while(timeout--);
		if (retry++ > 0xffff) {
			log_error("failed");
		}
	}
}

static int miiphy_try_speed(struct fec_priv *fec, int speed) {
	uint32_t reg;

	fec_mdio_write(fec, MII_CTRL1000, net_is_1000(speed) ?
			ADVERTISE_1000FULL | ADVERTISE_1000HALF : 0);

	fec_mdio_write(fec, MII_ADVERTISE, net_speed_to_adv(speed));

	reg = fec_mdio_read(fec, MII_BMCR);
	fec_mdio_write(fec, MII_BMCR, reg | BMCR_ANRESTART);

	miiphy_wait_aneg(fec);

	return adv_to_net_speed(fec_mdio_read(fec, MII_LPA) &
				fec_mdio_read(fec, MII_ADVERTISE));
}

static void miiphy_start_aneg(struct fec_priv *fec) {
	/* Wake up from sleep if necessary
	 * Reset PHY, then delay 300ns */
	miiphy_reset(fec);

	fec_mdio_write(fec, MII_BMCR, BMCR_ANENABLE);
#if FEC_SPEED == 1000
	if (miiphy_try_speed(fec, NET_1000FULL)) {
		log_boot("\t1000 Mbps FULL\n");
	} else {
		log_error("Autonegotiation failed for 1000 mbps FULL");
	}
#elif FEC_SPEED == 100
	if (miiphy_try_speed(fec, NET_100FULL)) {
		log_boot("\t100 Mbps FULL\n");
	} else {
		log_error("Autonegotiation failed for 1000 mbps FULL");
	}
#elif FEC_SPEED == 10
	if (miiphy_try_speed(fec, NET_10FULL)) {
		log_boot("\t10 Mbps FULL\n");
	} else {
		log_error("Autonegotiation failed for 10 mbps FULL");
	}
#else
	int ret;
	if ((ret = miiphy_try_speed(fec, NET_1000FULL | NET_1000HALF))) {
		if (ret & NET_1000FULL) {
			log_boot("\t1000 Mpbs FULL");
		} else {
			log_boot("\t1000 Mpbs HALF");
		}
	} else {
		ret = miiphy_try_speed(fec, NET_10HALF | NET_10FULL |
				NET_100HALF | NET_100FULL);

		if (ret & NET_100FULL) {
			log_boot("\t100 Mbps FULL\n");
		} else if (ret & NET_100HALF) {
			log_boot("\t100 Mbps HALF\n");
		} else if (ret & NET_10FULL) {
			log_boot("\t10 Mbps FULL\n");
		} else if (ret & NET_10HALF) {
			log_boot("\t10 Mbps HALF\n");
		} else {
			log_boot("Auto negotiatie error");
		}
	}
#endif
}

void fec_mdio_init(struct fec_priv *fec) {
	mdio_detect_phy(fec);
	miiphy_start_aneg(fec);
}
