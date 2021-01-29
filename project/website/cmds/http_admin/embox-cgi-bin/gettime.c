/**
 * @file
 * @brief CGI script to show current time/date
 *
 * @date 29.01.2021
 * @author Alexander Kalmuk
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char buf[128];
	char *pbuf;
	struct timeval tv;
	time_t time;

	printf(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
	);

	while (1) {
		pbuf = buf;

		pbuf += sprintf(pbuf, "data: {\"time\" : \"");

		gettimeofday(&tv, NULL);
		time = tv.tv_sec;
		ctime_r(&time, pbuf);

		strcat(pbuf, "\"}\n\n");

		if (0 > printf("%s", buf)) {
			break;
		}

		sleep(1);
	}

	return 0;
}
