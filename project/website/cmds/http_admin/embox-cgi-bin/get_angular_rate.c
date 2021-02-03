/**
 * @file
 * @brief Stub to simulate light sensor
 *
 * @date 02.02.2021
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <drivers/sensors/l3g4200d.h>

int main(int argc, char *argv[]) {
	unsigned int x, y, z;

	printf(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
	);

	while (1) {
		x = l3g4200d_get_angular_rate_x() / 100;
		y = l3g4200d_get_angular_rate_y() / 100;
		z = l3g4200d_get_angular_rate_z() / 100;

		if (0 > printf("data: {\"rate\" : \"x:%d y:%d z:%d\"}\n\n", x, y, z)) {
			break;
		}

		sleep(1);
	}

	return 0;
}
