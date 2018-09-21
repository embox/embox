/**
 * @file phy.h
 * @brief Common PHY config and auto negotiate
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.05.2018
 */

#ifndef NET_PHY_H_
#define NET_PHY_H_

#include <net/mii.h>
#include <net/netdevice.h>

extern int phy_detect(struct net_device *dev);
extern int phy_reset(struct net_device *dev);
extern int phy_wait_autoneg(struct net_device *dev);
extern int phy_try_speed(struct net_device *dev, int speed);
extern int phy_autoneg(struct net_device *dev, int fixed_speed);
extern int phy_init(struct net_device *dev);

#endif /* NET_PHY_H_ */
