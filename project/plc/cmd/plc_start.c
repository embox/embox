/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.12.24
 */

#include <stdio.h>

#include <plc/core.h>

int main(int argc, char **argv) {
	int err;

	if (plc_is_started()) {
		printf("PLC is already running\n");
		return 0;
	}

	err = plc_start();

	return err;
}
