/**
 * @file 
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Daria Pechenova
 * @version
 * @date 18.07.2025
 */

 #include <util/log.h>

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

#include <libs/ism43362.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#define WIFI_NAME "Embox2.4"
#define WIFI_PASSWORD "pX6tsPeEN6"
#define BUFFER_SIZE 2048

typedef struct {
    const char *command;
	const char *command_value;
	// const int command_len;
} AT_Command;

static const AT_Command wifi_setup_commands[] = {
	{"C1=Embox2.4", WIFI_NAME},
	{"C2=pX6tsPeEN6", WIFI_PASSWORD},
	{"C3=4", "4"},
	{"C4=1", "1"},
	{"C0", NULL},
	{NULL, NULL}
};

EMBOX_UNIT_INIT(eswifi_init);

static int eswifi_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

static int eswifi_open(struct net_device *dev) {
	int wifi_request;
	char rx_buffer[BUFFER_SIZE];
	// char tx_buffer[256];
	// buffer[BUFFER_SIZE - 1]=0;
	log_info("Starting Wi-Fi initialization");
	int result = 0;
    if (result != 0) {
        // printf("ISM43362 initialization error %d\n", result);
        return -1;
    } 	
	if (result > 0){
	for (int i = 0; wifi_setup_commands[i].command != NULL; i++){
		wifi_request = ism43362_exchange((char *)wifi_setup_commands[i].command, strlen(wifi_setup_commands[i].command), rx_buffer, sizeof(rx_buffer));
		// printf("Response (%d bytes): [", wifi_request);
		if (wifi_request < 0) {
			printf("ism43362_exchange error #%d\n", wifi_request);
		}
		usleep(100000);
	}}
	// gpio_set(2, 1 << 9, 1);

	log_info("Wi-Fi initialization finished");
	return 0;
}

static int eswifi_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver eswifi_drv_ops = {
	.xmit = eswifi_xmit,
	.start = eswifi_open,

	.set_macaddr = eswifi_set_macaddr,
};

static int eswifi_init(void) {
	struct net_device *nic;

	nic = etherdev_alloc(1);
	if (NULL == nic) {
		return -ENOMEM;
	}

	nic->drv_ops = &eswifi_drv_ops;
	return inetdev_register_dev(nic);
}
