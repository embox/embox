#include <stdio.h>
#include <string.h>
#include <mqtt_udp.h>

#define MSG_LEN 256

int main(int argc, const char **argv) {
	char topic[MSG_LEN], value[MSG_LEN];

	strcpy(topic, "/temp");
	strcpy(value, "40");
	int rc = mqtt_udp_send_publish(topic, value);
	
	if (rc)
		printf("rc = %d\n", rc);

	return 0;
}
