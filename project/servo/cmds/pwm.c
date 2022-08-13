/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2022-07-30
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/time/ktime.h>
#include <kernel/thread.h>

#include <drivers/servo/servo.h>

int main(int argc, char **argv) {
	struct servo_dev *servo = servo_dev_by_id(0);
	if (servo == NULL) {
	  printf("No servo devices are registered\n");
	  return 0;
	}

	servo->servo_ops->init(servo);
	int pos = 0;

	if (argc > 1) {
		pos = atoi(argv[1]);
		servo->servo_ops->set_pos(servo, pos);
		return 0;
	}

	while (1) {
		pos++;
		servo->servo_ops->set_pos(servo, pos % 100);
		ksleep(10);
	}

	return 0;
}
