/**
 * @file
 *
 * @date 13.01.2017
 * @author Anton Bondarev
 */

#ifndef MII_H_
#define MII_H_

#define MII_BMCR        0x0 /* Basic Mode Configuration */
#define MII_BMSR        0x1 /* Basic Mode Status */
#define MII_PHYSID1     0x2 /* PHY ID #1 */
#define MII_PHYSID2     0x3 /* PHY ID #2 */
#define MII_ADVERTISE   0x4 /* AutoNegotation Advertisement */
#define MII_LPA         0x5 /* Link Partner Ability */
#define MII_EXPANSION   0x6 /* AutoNegotation Expansion */

#endif /* MII_H_ */
