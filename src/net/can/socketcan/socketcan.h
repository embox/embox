/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#ifndef NET_CAN_SOCKETCAN_H_
#define NET_CAN_SOCKETCAN_H_

#include <lib/libds/array_spread.h>
#include <net/can.h>

#define CAN_IMPL_SOCKETCAN 1

#define __CAN_CONTROLLER_REGISTER(_can_controller)                                \
	ARRAY_SPREAD_DECLARE(struct can_controller *const, __can_socketcan_registry); \
	ARRAY_SPREAD_ADD(__can_socketcan_registry, &_can_controller)

#endif /* NET_CAN_SOCKETCAN_H_ */
