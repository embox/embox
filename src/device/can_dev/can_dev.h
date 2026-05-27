/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#ifndef DEVICE_CAN_DEV_H_
#define DEVICE_CAN_DEV_H_

#include <linux/can.h>

#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <lib/libds/array_spread.h>
#include <util/macro.h>

#include <config/embox/device/can_dev.h>

#define CAN_RX_BUFF_SIZE \
	OPTION_MODULE_GET(embox__device__can_dev, NUMBER, rx_buff_size)

#define CAN_FD_FRAMES \
	OPTION_MODULE_GET(embox__device__can_dev, BOOLEAN, fd_frames)

#if CAN_FD_FRAMES
typedef struct canfd_frame can_frame_t;
#else
typedef struct can_frame can_frame_t;
#endif

#define CAN_DEVICE_DEF(_name, _ops, _priv, _dev_id)                         \
	static struct can_dev _name = {                                         \
	    .cdev = CHAR_DEV_INIT(_name.cdev,                                   \
	        MACRO_STRING(MACRO_CONCAT(can, _dev_id)), &__can_char_dev_ops), \
	    .ops = _ops,                                                        \
	    .priv = _priv,                                                      \
	    .dev_id = _dev_id,                                                  \
	};                                                                      \
	CHAR_DEV_REGISTER((struct char_dev *)&_name)

struct can_ops;

struct can_dev {
	struct char_dev cdev;
	struct waitq waitq;
	const struct can_ops *ops;
	void *priv;
	int dev_id;
	int rx_buff_cnt;
	can_frame_t rx_buff[CAN_RX_BUFF_SIZE];
};

struct can_ops {
	int (*open)(struct can_dev *can);
	void (*close)(struct can_dev *can);
	void (*eoi)(struct can_dev *can);
	void (*irq_en)(struct can_dev *can);
	void (*irq_dis)(struct can_dev *can);
	int (*hasrx)(struct can_dev *can);
	int (*send)(struct can_dev *can, can_frame_t *frame);
	int (*receive)(struct can_dev *can, can_frame_t *frame);
	// int (*set_filter)(struct can_dev *can, struct can_filter *filter);
};

extern const struct char_dev_ops __can_char_dev_ops;

extern irq_return_t can_dev_irq_handler(unsigned int irq_num, void *data);

#endif /* DEVICE_CAN_DEV_H_ */
