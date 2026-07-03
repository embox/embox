/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#include <poll.h>

#include <drivers/can_buff.h>
#include <drivers/can_dev.h>
#include <drivers/char_dev.h>

int can_dev_receive(struct can_dev *can, void *frame) {
	return can_buff_write(can->buff, frame, can->mtu);
}

int can_dev_wait(struct can_dev *can) {
	return char_dev_wait((struct char_dev *)can, POLLIN | POLLERR);
}

void can_dev_notify(struct can_dev *can) {
	if (!can_buff_empty(can->buff)) {
		char_dev_notify((struct char_dev *)can);
	}
}
