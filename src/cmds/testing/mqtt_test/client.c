#include "mqtt_test.h"

int main(int argc, char *argv[]) {
	/* connection and socket listening */
    struct addrinfo hints;
    int sockfd;
	/* message sending */
    size_t len;
    ssize_t nwrite;
    char buf[BUF_SIZE], msg[BUF_SIZE];
	/* logging */
	FILE *fd = fopen("log/client.log", "a");
	char timestamp[BUF_SIZE];
	time_t timer;
	struct tm *cur_time;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */
	
	sockfd = sock_connect("10.0.2.16", TEST_PORT, &hints);
	if (sockfd < 0) {
		timer = time(NULL);
		cur_time = localtime(&timer);
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %I:%M:%S %p", cur_time);
		
		fprintf(fd, "[%s] Failed to connect to server: %s\n",
				timestamp, strerror(sockfd));
		return sockfd;
	}
	
	for (int i = atoi(argv[1]); i <= 100; i+=2) {
		sprintf(msg, "ping%i", i);
		len = strlen(msg) + 1;

		timer = time(NULL);
		cur_time = localtime(&timer);
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %I:%M:%S %p", cur_time);

		if (len > BUF_SIZE) {
			fprintf(fd, "[%s] Message too long.", timestamp);
			sleep(i);
			continue;
		}

		nwrite = write(sockfd, msg, len);
		if (nwrite != len) {
			fprintf(fd, "[%s] partial/failed write.\n", timestamp);
			sleep(i);
			continue;
		}

		fprintf(fd, "[%s] Sent %ld bytes to server. Message: %s\n",
					 timestamp, (long) nwrite, msg);

		nwrite = read(sockfd, buf, BUF_SIZE);
		if (nwrite == -1) {
			fprintf(fd, "[%s] Read failure: %s\n", timestamp, strerror(errno));
			sleep(i);
			continue;
		}

		fprintf(fd, "[%s] Recieved %ld bytes from server. Message: %s\n",
					timestamp, (long) nwrite, msg);
		
		sleep(i);
	}

	timer = time(NULL);
	cur_time = localtime(&timer);
	strftime(timestamp, BUF_SIZE, "%d-%m-%Y %I:%M:%S %p", cur_time);
	fprintf(fd, "[%s] Transmit completed.\n", timestamp);

	return 0;
}
