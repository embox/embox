/**
 * @file
 *
 * @date Jan 21, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_MCST_DP83865_PHY_H_
#define SRC_DRIVERS_NET_MCST_DP83865_PHY_H_


/* "*" - if HARD == 1 then "RW", and "R" in other case. In the "HARD == 0"
* case e1000 working mode is specified by PHY  */

/* Some of PHY (DP83865) registers */
#define PHY_AUX_CTRL  0x12  /* Auxiliary Control Register */
#define PHY_LED_CTRL  0x13  /* LED Control Register */
#define PHY_BIST_CFG2 0x1a  /* BIST configuration Register 2 */

/* Some of PHY_AUX_CTRL's fields */
#define RGMII_EN_1 (1 << 13)
#define RGMII_EN_0 (1 << 12)
/* This two fields enable either RGMII or GMII/MII */
	/* 1:1 - RGMII 3com mode
	 * 1:0 - RGMII Hp mode
	 * 0:1 - GMII/MII mode
	 * 0:0 - GMII/MII mode
	 */

/* Some of PHY_LED_CTRL's fields */
#define RED_LEN_EN (1 << 5) /* Reduced LED enable */

/* Some of PHY_BIST_CFG2's fields */
#define LINK_SEL   (1 << 0) /* Link/Link-ACT selector */
/* When RED_LEN_EN bit is enabled:
 * LINK_SEL = 1 - 10M Link LED displays 10/100/1000 Link
 * LINK_SEL = 0 - 10M Link LED displays 10/100/1000 Link and ACT
*/

#endif /* SRC_DRIVERS_NET_MCST_DP83865_PHY_H_ */
