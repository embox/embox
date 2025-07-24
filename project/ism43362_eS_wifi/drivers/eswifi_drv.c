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

#include <util/macro.h>

#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

#include <libs/ism43362.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#define WIFI_NAME         MACRO_STRING(OPTION_GET(STRING,ssid))
#define WIFI_PASSWORD     MACRO_STRING(OPTION_GET(STRING,passw))

#define BUFFER_SIZE 2048

typedef struct {
    const char *command;
	const char *command_value;
	// const int command_len;
} AT_Command;

static const AT_Command wifi_setup_commands[] = {
	{"C1=" WIFI_NAME "\r", WIFI_NAME},
	{"C2=" WIFI_PASSWORD "\r", WIFI_PASSWORD},
	{"C3=4\r", "4"},
	{"C4=1\r", "1"},
	{"C0\r", NULL},
	{"Z5\r", NULL},
	{NULL, NULL}
};

EMBOX_UNIT_INIT(eswifi_init);

static int eswifi_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

static int eswifi_open(struct net_device *dev) {
	int wifi_request;
	char rx_buffer[BUFFER_SIZE];
	char *ip;
	uint32_t ip_num;
	struct in_device *iface;
	unsigned char mac_bytes[6];

	log_info("Starting Wi-Fi initialization");
	int result = ism43362_init();
    if (result != 0) {
		log_info("ISM43362 initialization error");
        return -1;
    } 	

	for (int i = 0; wifi_setup_commands[i].command != NULL; i++){
		wifi_request = ism43362_exchange((char *)wifi_setup_commands[i].command, strlen(wifi_setup_commands[i].command), rx_buffer, sizeof(rx_buffer));
		if (wifi_request < 0) {
			log_info("ism43362_exchange error");
		}

		log_info(wifi_setup_commands[i].command);
		log_info(rx_buffer);
		
		if (wifi_setup_commands[i].command[0] == 'C' && wifi_setup_commands[i].command[1]== '0')
		{
			ip = strtok(rx_buffer, ",");
			ip = strtok(NULL, ",");
			if (ip){
				ip_num = inet_addr(ip); 
				iface = inetdev_get_by_dev(dev);
				inetdev_set_addr(iface, ip_num);
			}
		} else if (wifi_setup_commands[i].command[0] == 'Z' && wifi_setup_commands[i].command[1] == '5') {
			
			if (sscanf(strchr(rx_buffer, '\n') + 1, 
					"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
					&mac_bytes[0], &mac_bytes[1], &mac_bytes[2],
					&mac_bytes[3], &mac_bytes[4], &mac_bytes[5]) == 6) {
				
				memcpy(dev->dev_addr, mac_bytes, 6);
				log_info("MAC set");  
			} else {
				log_error("Invalid MAC format in response: %s", rx_buffer);
				return -1;
			}
		}
		memset(rx_buffer, '\0', BUFFER_SIZE);
	}
	
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
