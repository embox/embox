#include "mqtt_test.h"

int main(int argc, char *argv[]) {
	/* connection and socket listening */
    struct addrinfo hints;
    int sockfd;
	/* message sending */
	int i;
    size_t len;
    ssize_t nwrite;
    char buf[BUF_SIZE], msg[BUF_SIZE];
	/* logging */
	// FILE *fd = fopen("log/client.log", "a");
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
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %T", cur_time);
		
		printf("[%s] Failed to connect to server: %s\n",
				timestamp, strerror(sockfd));
		// fprintf(fd, "[%s] Failed to connect to server: %s\n",
		// 		timestamp, strerror(sockfd));
		return sockfd;
	}
	
	i = (argc > 1)? atoi(argv[1]) : 0;
	for (; i <= 64; i+=1) {
		sprintf(msg, "ping%i", i);
		len = strlen(msg) + 1;
		int sleep_time; 

		timer = time(NULL);
		cur_time = localtime(&timer);
		strftime(timestamp, BUF_SIZE, "%d-%m-%Y %T", cur_time);

		srandom(timer);
		sleep_time = random() % 5;

		if (len > BUF_SIZE) {
			// fprintf(fd, "[%s] Message too long.", timestamp);
			sleep(sleep_time);
			continue;
		}

		nwrite = write(sockfd, msg, len);
		if (nwrite != len) {
			printf("[%s] partial/failed write.\n", timestamp);
			// fprintf(fd, "[%s] partial/failed write.\n", timestamp);
			sleep(sleep_time);
			continue;
		}

		printf("[%s] Sent %ld bytes to server. Message: %s\n",
				timestamp, (long) nwrite, msg);
		// fprintf(fd, "[%s] Sent %ld bytes to server. Message: %s\n",
		// 			 timestamp, (long) nwrite, msg);

#if 0
		nwrite = read(sockfd, buf, BUF_SIZE);
		if (nwrite == -1) {
			printf("[%s] Read failure: %s\n", timestamp, strerror(errno));
			// fprintf(fd, "[%s] Read failure: %s\n", timestamp, strerror(errno));
			sleep(i);
			continue;
		}

		printf("[%s] Recieved %ld bytes from server. Message: %s\n",
				timestamp, (long) nwrite, msg);
		// fprintf(fd, "[%s] Recieved %ld bytes from server. Message: %s\n",
		// 			timestamp, (long) nwrite, msg);
#endif
		
		sleep(sleep_time);
	}

	timer = time(NULL);
	cur_time = localtime(&timer);
	strftime(timestamp, BUF_SIZE, "%d-%m-%Y %T", cur_time);
	printf("[%s] Transmit completed.\n", timestamp);
	// fprintf(fd, "[%s] Transmit completed.\n", timestamp);

	return 0;
}
