/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#include <assert.h>
#include <linux/can.h>
#include <poll.h>
#include <sys/types.h>

#include <drivers/can_dev.h>
#include <drivers/char_dev.h>
#include <kernel/irq.h>

irq_return_t can_dev_irq_handler(unsigned int irq_num, void *data) {
	struct can_dev *dev;
	can_frame_t *frame;

	dev = (struct can_dev *)data;
	assert(dev->rx_buff_cnt <= CAN_RX_BUFF_SIZE);

	while (dev->ops->hasrx(dev)) {
		if (dev->rx_buff_cnt == CAN_RX_BUFF_SIZE) {
			dev->ops->irq_dis(dev);
			return IRQ_HANDLED;
		}

		frame = &dev->rx_buff[dev->rx_buff_cnt];

		if (0 == dev->ops->receive(dev, frame)) {
			++dev->rx_buff_cnt;
		}
	}

	dev->ops->eoi(dev);

	if (dev->rx_buff_cnt > 0) {
		waitq_wakeup_all(&dev->waitq);
	}

	return IRQ_HANDLED;
}
