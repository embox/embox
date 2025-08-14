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
#include <net/cfg80211.h>

#include <libs/ism43362.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#define ESWIFI_SECURITY_OPEN  0
#define ESWIFI_SECURITY_WPA2  4

#define WIFI_NAME          MACRO_STRING(OPTION_GET(STRING,ssid))
#define WIFI_PASSWORD      MACRO_STRING(OPTION_GET(STRING,passw))
#define WIFI_SECURITYTYPE  ESWIFI_SECURITY_WPA2

#define BUFFER_SIZE     1024

typedef struct {
    const char *command;
	const char *command_value;
	// const int command_len;
} AT_Command;

static const AT_Command wifi_setup_commands[] = {
//	{"C1=" WIFI_NAME "\r", WIFI_NAME},
//	{"C2=" WIFI_PASSWORD "\r", WIFI_PASSWORD},
//	{"C3=4\r", "4"},
//	{"C4=1\r", "1"},
	{"C0\r", NULL},
	{"C?\r", NULL},
	{"Z5\r", NULL},
	{NULL, NULL}
};

EMBOX_UNIT_INIT(eswifi_init);

static int eswifi_xmit(struct net_device *dev, struct sk_buff *skb) {

	return 0;
}

static int eswifi_connect(struct wiphy *wiphy, struct net_device *dev,
						struct cfg80211_connect_params *sme) {
	int wifi_request;
	char buf[64];
	char rx_buffer[256];
	int err;
	char *essid = (char *)sme->ssid; //WIFI_NAME
	char *pwd = (char *)sme->key; //WIFI_PASSWORD;
	char *ip;
	uint32_t ip_num;
	struct in_device *iface;
	unsigned char mac_bytes[6];

	iface = inetdev_get_by_dev(dev);

	/* Set SSID */
	snprintf(buf, sizeof(buf), "C1=%s\r", essid);
	//snprintf(buf, sizeof(buf),"%s\r", cmd_c1);
	err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));
	if (err < 0) {
		log_error("Unable to set SSID");
		return 0;
	}

	/* Set passphrase */
	snprintf(buf, sizeof(buf), "C2=%s\r", pwd);
	//snprintf(buf, sizeof(buf),"%s\r", cmd_c2);
	err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));
	if (err < 0) {
		log_error("Unable to set passphrase");
		return 0;
	}

	/* Set Security type */
	snprintf(buf, sizeof(buf), "C3=%u\r", WIFI_SECURITYTYPE);
	err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));
	if (err < 0) {
		log_error("Unable to set Security type");
		return 0;
	}

	/* Join Network (DHCP enabled)*/
	snprintf(buf, sizeof(buf), "C4=%u\r", 1/* connect*/);
	err = ism43362_exchange((char *)buf, strlen(buf), rx_buffer, sizeof(rx_buffer));
	if (err < 0) {
		log_error("Unable connect");
		return 0;
	}

	log_debug("%s", rx_buffer);

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

				inetdev_set_addr(iface, ip_num);
			}
		}  else if (wifi_setup_commands[i].command[0] == 'C' && wifi_setup_commands[i].command[1] == '?') {
			char *token;
			char *mask = NULL;
			int field_num = 0;

			token = strtok(rx_buffer, ","); 
			while (token != NULL) {
				if (field_num == 6) { 
					mask = token;
					break;
				}
				token = strtok(NULL, ",");
				field_num++;
			}
			if (mask != NULL) {
				inetdev_set_mask(iface, (inet_addr(mask)));
			}

		} else if (wifi_setup_commands[i].command[0] == 'Z' && wifi_setup_commands[i].command[1] == '5') {
			
			if (sscanf(strchr(rx_buffer, '\n') + 1, 
					"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
					&mac_bytes[0], &mac_bytes[1], &mac_bytes[2],
					&mac_bytes[3], &mac_bytes[4], &mac_bytes[5]) == 6) {
				
				memcpy(dev->dev_addr, mac_bytes, 6);
			} else {
				log_error("Invalid MAC format in response: %s", rx_buffer);
				return -1;
			}
		}
		memset(rx_buffer, '\0', sizeof(rx_buffer));
	}
	
	return 0;
}

static int eswifi_open(struct net_device *dev) {
	int res;

	log_info("Starting Wi-Fi initialization");

	res = ism43362_init();
    if (res != 0) {
		log_error("ISM43362 initialization failed error %d", res);
        return -1;
    }

	log_info("Wi-Fi initialization finished OK");
	
	return 0;
}

static int eswifi_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver eswifi_netdev_ops = {
	.xmit = eswifi_xmit,
	.start = eswifi_open,

	.set_macaddr = eswifi_set_macaddr,
};


static inline
int eswifi_parse_scan_list(char *buf, int buf_len,
								struct cfg80211_ssid *ssids, int n_ssids) {
	int i;
	int j;

	for (i = 0; i < n_ssids; i++) {
		/* fmt => #001,"SSID",MACADDR,RSSI,BITRATE,MODE,SECURITY,BAND,CHANNEL */
		while(('#' != *buf) && ('\0' != *buf) && (buf_len > 0) ) {
			buf_len --;
			buf++;
		};
		if (*buf != '#') {
			break;
		}
		buf += 6; /* sizeof("001,\"") */
		buf_len -= 6;
		for (j = 0; j < sizeof(ssids[i].ssid); j++) {
			if (*buf == '\"') {
				while(('\n' != *buf) && ('\0' != *buf) && (buf_len > 0)) {
					buf_len --;
					buf++;
				};
				break;
			}
			ssids[i].ssid[j] = *buf++;
			buf_len --;
		}
	}

	return i;
}

static int eswifi_scan(struct wiphy *wiphy, struct cfg80211_scan_request *req) {
	char cmd[] = "F0\r";
	char rx_buffer[2048];
	int err;

	err = ism43362_exchange(cmd, sizeof(cmd), rx_buffer, sizeof(rx_buffer));
	if (err < 0) {
		log_error("failed scan");
		return 0;
	}

	log_debug("%s", rx_buffer);

	req->n_ssids = eswifi_parse_scan_list(rx_buffer, err, req->ssids, req->n_ssids);

	return 0;
}

static const struct cfg80211_ops eswifi_cfg80211_ops = {
	.connect = eswifi_connect,
	.scan = eswifi_scan,
};

static struct wireless_dev eswifi_wdev;

static int eswifi_init(void) {
	struct net_device *nic;
	int res;
	struct wiphy *wiphy;

	res = 0;

	nic = etherdev_alloc(0);
	if (NULL == nic) {
		return -ENOMEM;
	}

	wiphy = wiphy_new_nm(&eswifi_cfg80211_ops, 0, "wlan0");
	if (NULL == wiphy) {
		return -ENOMEM;
	}

	nic->nd_ieee80211_ptr = &eswifi_wdev;
	eswifi_wdev.netdev = nic;
	eswifi_wdev.wiphy = wiphy;

	nic->drv_ops = &eswifi_netdev_ops;

	res = inetdev_register_dev(nic);
	if (res != 0) {
		return res;
	}

	res = cfg80211_register_netdevice(nic);

	return res;
}
