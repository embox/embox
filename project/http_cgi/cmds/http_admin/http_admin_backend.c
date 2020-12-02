/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.07.2014
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <ifaddrs.h>

#include <net/inetdevice.h>
#include <net/util/macaddr.h>
#include <cJSON.h>
#include <embox/cmd.h>
#include <kernel/printk.h>
#include <hal/arch.h>

static char *http_admin_build_iface_list(void) {
        struct ifaddrs *i_ifa, *ifa = NULL;
	cJSON *iface_array;
       	char *json_list;

	iface_array = cJSON_CreateArray();
	if (!iface_array) {
		goto outerr;
	}

        if (-1 == getifaddrs(&ifa)) {
		goto outerr;
        }

        for (i_ifa = ifa; i_ifa != NULL; i_ifa = i_ifa->ifa_next) {
		struct in_device *iface_dev;
		struct sockaddr *iaddr = i_ifa->ifa_addr;
		cJSON *iface_obj;
		char buf[64];

                if (iaddr == NULL || iaddr->sa_family != AF_INET) {
			continue;
		}

		if (0 == strcmp(i_ifa->ifa_name, "lo")) {
			continue;
		}

		iface_obj = cJSON_CreateObject();
		if (!iface_obj) {
			goto outerr;
		}
		cJSON_AddItemToArray(iface_array, iface_obj);

		/* FIXME cJSON_Add* are not report failed memory allocation */

		cJSON_AddStringToObject(iface_obj, "name", i_ifa->ifa_name);

		cJSON_AddStringToObject(iface_obj, "ip",
				inet_ntop(iaddr->sa_family,
					&((struct sockaddr_in *) iaddr)->sin_addr,
					buf,
					sizeof(buf)));

		cJSON_AddStringToObject(iface_obj, "netmask",
				inet_ntop(i_ifa->ifa_netmask->sa_family,
					&((struct sockaddr_in *) i_ifa->ifa_netmask)->sin_addr,
					buf,
					sizeof(buf)));

		iface_dev = inetdev_get_by_name(i_ifa->ifa_name);
		if (!iface_dev) {
			goto outerr;
		}
		macaddr_print((unsigned char *) buf, (unsigned char *) iface_dev->dev->dev_addr);
		cJSON_AddStringToObject(iface_obj, "mac", buf);
        }

	json_list = cJSON_PrintUnformatted(iface_array);

	cJSON_Delete(iface_array);
	freeifaddrs(ifa);
	return json_list;
outerr:
	cJSON_Delete(iface_array);
	freeifaddrs(ifa);
	return strdup("{}");
}

static char *cJSON_GetObjectString(cJSON *obj, const char *name) {
	cJSON *item;

	assert(obj->type == cJSON_Object);
	item = cJSON_GetObjectItem(obj, name);
	assert(item);
	assert(item->type == cJSON_String);
	return item->valuestring;
}

static void http_admin_post(char *post_data) {
	struct in_device *iface_dev;
	struct in_addr if_addr, if_netmask;
	unsigned char if_hwaddr[MAX_ADDR_LEN];
	const char *action;
	cJSON *post_json;

	post_json = cJSON_Parse(post_data);

	if (!post_json) {
		goto outerr;
	}

	action = cJSON_GetObjectString(post_json, "action");
	if (!strcmp(action, "iface_update")) {
		cJSON *iface_desc = cJSON_GetObjectItem(post_json, "data");

		iface_dev = inetdev_get_by_name(cJSON_GetObjectString(iface_desc, "name"));
		if (!iface_dev) {
			goto outerr;
		}

		if (1 != inet_pton(AF_INET, cJSON_GetObjectString(iface_desc, "ip"), &if_addr)) {
			goto outerr;
		}
		if (inetdev_set_addr(iface_dev, if_addr.s_addr)) {
			goto outerr;
		}

		if (1 != inet_pton(AF_INET, cJSON_GetObjectString(iface_desc, "netmask"), &if_netmask)) {
			goto outerr;
		}
		if (inetdev_set_mask(iface_dev, if_netmask.s_addr)) {
			goto outerr;
		}

		if (!macaddr_scan((unsigned char *)cJSON_GetObjectString(iface_desc, "mac"), if_hwaddr)) {
			goto outerr;
		}
		if (netdev_set_macaddr(iface_dev->dev, if_hwaddr)) {
			goto outerr;
		}

		if (!system("flash_settings store net")) {
			printk("Net configuration is saved succesffully\n");
			printk("\tRebooting now to apply new net config...\n");
			arch_shutdown(ARCH_SHUTDOWN_MODE_REBOOT);
		} else {
			printk("Net configuration saving failed\n");
		}
	}

outerr:
	cJSON_Delete(post_json);
}

int main(int argc, char *argv[]) {
	char *method;

	printf(
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n", 200, "OK", "application/json");

	method = getenv("REQUEST_METHOD");
	if (0 == strcmp("GET", method)) {
		char *list = http_admin_build_iface_list();
		printf("%s\n", list);
		free(list);
	} else if (0 == strcmp("POST", method)) {
		char buf[256];
		size_t clen = atoi(getenv("CONTENT_LENGTH"));

		if (clen < sizeof(buf) && 0 < fread(buf, clen, 1, stdin)) {
			buf[clen] = '\0';
			http_admin_post(buf);
		}
	}

	fflush(stdout);
	return 0;
}
