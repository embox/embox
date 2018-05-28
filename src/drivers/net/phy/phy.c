/**
 * @file phy.c
 * @brief Common PHY config and auto negotiate
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.05.2018
 */

#include <net/phy.h>
#include <util/log.h>

static int phy_read(struct net_device *dev, int reg) {
	assert(dev);
	assert(dev->drv_ops);
	assert(dev->drv_ops->mdio_read);

	return dev->drv_ops->mdio_read(dev, reg);
}

static int phy_write(struct net_device *dev, int reg, int data) {
	assert(dev);
	assert(dev->drv_ops);
	assert(dev->drv_ops->mdio_write);

	return dev->drv_ops->mdio_write(dev, reg, data);
}

static void set_phyid(struct net_device *dev, int phyid) {
	assert(dev);
	assert(dev->drv_ops);
	assert(dev->drv_ops->set_phyid);

	return dev->drv_ops->set_phyid(dev, phyid);
}

int phy_detect(struct net_device *dev) {
	assert(dev);
	for (int i = 0; i < 32; i++) {
		set_phyid(dev, i);
		if (phy_read(dev, MII_PHYSID1) != 0xffff) {
			log_debug("Detected phyid=%d", i);
			return 0;
		}
	}

	return -1;
}

int phy_reset(struct net_device *dev) {
	uint32_t reg, retry = 0;
	assert(dev);
	log_debug("Reset PHY");
	reg = phy_read(dev, MII_BMCR);
	phy_write(dev, MII_BMCR, reg | BMCR_RESET);

	while (phy_read(dev, MII_BMCR) & BMCR_RESET) {
		int timeout = 0xfffffff;
		while(timeout--);
		if (retry++ > 0xffff) {
			log_error("PHY reset failed");
			return -1;
		}
	}

	return 0;
}

int phy_wait_autoneg(struct net_device *dev) {
	int retry = 0;

	while (!(phy_read(dev, MII_BMSR) & BMSR_LSTATUS)) {
		if (retry++ > 10000) {
			log_error("Autonegotiation timeout");
			return -1;
		}

		int timeout = 0xffffff;
		while(timeout--);
	}

	return 0;
}

int phy_try_speed(struct net_device *dev, int speed) {
	uint32_t reg;
	phy_write(dev, MII_CTRL1000, net_is_1000(speed) ?
			ADVERTISE_1000FULL | ADVERTISE_1000HALF : 0);

	phy_write(dev, MII_ADVERTISE, net_speed_to_adv(speed));

	reg = phy_read(dev, MII_BMCR);
	phy_write(dev, MII_BMCR, reg | BMCR_ANRESTART);

	phy_wait_autoneg(dev);

	return adv_to_net_speed(phy_read(dev, MII_LPA) &
				phy_read(dev, MII_ADVERTISE));
}

int phy_autoneg(struct net_device *dev, int fixed_speed) {
	int ret;
	assert(dev);
	/* Wake up from sleep if necessary
	 * Reset PHY, then delay 300ns */
	phy_reset(dev);

	phy_write(dev, MII_BMCR, BMCR_ANENABLE);

	switch (fixed_speed) {
	case 1000:
		if (phy_try_speed(dev, NET_1000FULL)) {
			log_boot("\t1000 Mbps FULL\n");
			dev->drv_ops->set_speed(dev, NET_1000FULL);
		}
		break;
	case 100:
		if (phy_try_speed(dev, NET_100FULL)) {
			log_boot("\t100 Mbps FULL\n");
			dev->drv_ops->set_speed(dev, NET_100FULL);
		}
		break;
	case 10:
		if (phy_try_speed(dev, NET_10FULL)) {
			log_boot("\t10 Mbps FULL\n");
			dev->drv_ops->set_speed(dev, NET_10FULL);
		}
		break;
	default:
	/* Find out best speed */
		if ((ret = phy_try_speed(dev, NET_1000FULL | NET_1000HALF))) {
			if (ret & NET_1000FULL) {
				log_boot("\t1000 Mpbs FULL");
				dev->drv_ops->set_speed(dev, NET_1000FULL);
			} else {
				log_boot("\t1000 Mpbs HALF");
				dev->drv_ops->set_speed(dev, NET_1000HALF);
			}
			return 0;
		} else {
			ret = phy_try_speed(dev, NET_10HALF | NET_10FULL |
						NET_100HALF | NET_100FULL);

			if (0 == (ret = net_top_speed(ret))) {
				goto err_out;
			}


			log_boot("\t%d Mbps %s\n", ret,
					net_is_fullduplex(ret) ? "FULL" : "HALF");

			assert(dev->drv_ops->set_speed);

			dev->drv_ops->set_speed(dev, ret);
		}
	}

	return 0;
err_out:
	log_error("Autonegotiation failed");
	return -1;
}

int phy_init(struct net_device *dev) {
	phy_detect(dev);
	phy_autoneg(dev, 0);

	return 0;
}
