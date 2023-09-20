/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 09.09.23
 */
#include <net/netdevice.h>
#include <drivers/common/memory.h>
#include <framework/mod/options.h>

#include <config/embox/driver/net/greth.h>

#define BASE_ADDR \
	OPTION_MODULE_GET(embox__driver__net__greth, NUMBER, base_addr)
#define IRQ_NUM OPTION_MODULE_GET(embox__driver__net__greth, NUMBER, irq_num)

void __greth_dev_init(struct net_device *dev) {
	dev->base_addr = BASE_ADDR;
	dev->irq = IRQ_NUM;
}

PERIPH_MEMORY_DEFINE(greth, BASE_ADDR, 0x1000);
