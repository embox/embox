/**
 * @file
 *
 * @date Mar 28, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_RCM_MGETH_RCM_MDIO_H_
#define SRC_DRIVERS_NET_RCM_MGETH_RCM_MDIO_H_


#define MDIO_ID            0x00
#define MDIO_VER           0x04
#define MDIO_STATUS        0x08
#define MDIO_IRQ_MASK      0x0C
#define MDIO_PHY_IRQ_STATE 0x10
#define MDIO_CONTROL       0x14
#define MDIO_ETH_RST_N     0x18
#define MDIO_FREQ_DIVIDER  0x1C
#define MDIO_EN            0x20

#define MDIO_ID_RESET             0x4F49444D
#define MDIO_VER_RESET            0x00640101
#define MDIO_STATUS_RESET         0x00
#define MDIO_IRQ_MASK_RESET       0x00
#define MDIO_PHY_IRQ_STATE_RESET  0x00
#define MDIO_CONTROL_RESET        0x00
#define MDIO_ETH_RST_N_RESET      0x00
#define MDIO_FREQ_DIVIDER_RESET   0x13
#define MDIO_EN_RESET             0x00


#define ETH_RST_N                 0x00
#define MDC_EN                    0x00
#define PHY_IRQ                   0x00
#define RD_IRQ                    0x01
#define WR_IRQ                    0x02

#define START_WR                  0x00
#define START_RD                  0x01
#define BUSY                      0x02
#define ADDR_PHY                  0x03
#define ADDR_REG                  0x08
#define CTRL_DATA                 0x10


extern void rcm_mdio_init(int dev_num);
extern void rcm_mdio_en(int dev_num, int en) ;
extern void rcm_mdio_write(int dev_num, uint32_t reg_addr,uint32_t write_data);

#endif /* SRC_DRIVERS_NET_RCM_MGETH_RCM_MDIO_H_ */
