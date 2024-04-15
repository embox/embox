/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.07.2014
 */

#include <arpa/inet.h>
#include <cJSON.h>
#include <ifaddrs.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <framework/mod/options.h>
#include <hal/platform.h>
#include <net/inetdevice.h>
#include <net/l3/route.h>
#include <net/util/macaddr.h>
#include <util/log.h>

#define USE_NETMANAGER OPTION_GET(BOOLEAN, use_netmanager)

static in_addr_t *iface_get_gateway(char *ifname) {
	struct rt_entry *rt;

	for (rt = rt_fib_get_first(); rt != NULL; rt = rt_fib_get_next(rt)) {
		if ((rt->rt_gateway != 0) && !strcmp(ifname, rt->dev->name)) {
			return &rt->rt_gateway;
		}
	}

	return 0;
}

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
		bool use_dhcp = false;

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
		        &((struct sockaddr_in *)iaddr)->sin_addr, buf, sizeof(buf)));

		cJSON_AddStringToObject(iface_obj, "netmask",
		    inet_ntop(i_ifa->ifa_netmask->sa_family,
		        &((struct sockaddr_in *)i_ifa->ifa_netmask)->sin_addr, buf,
		        sizeof(buf)));

		cJSON_AddStringToObject(iface_obj, "gateway",
		    inet_ntop(i_ifa->ifa_netmask->sa_family,
		        iface_get_gateway(i_ifa->ifa_name), buf, sizeof(buf)));

		iface_dev = inetdev_get_by_name(i_ifa->ifa_name);
		if (!iface_dev) {
			goto outerr;
		}
		macaddr_print((unsigned char *)buf,
		    (unsigned char *)iface_dev->dev->dev_addr);
		cJSON_AddStringToObject(iface_obj, "mac", buf);
#if USE_NETMANAGER
		snprintf(buf, sizeof(buf), "netmanager -d %s", i_ifa->ifa_name);
		switch (system(buf)) {
		case 0:  /* disabled */
		case -1: /* not available */
			break;
		case 1: /* enabled */
			use_dhcp = true;
			break;
		default:
			break;
		}
#endif
		log_debug("use_dhcp %d", use_dhcp);
		cJSON_AddBoolToObject(iface_obj, "useDhcp", use_dhcp);
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
	struct in_addr if_addr, if_netmask, if_gateway;
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

		log_debug("Action: iface update");

		iface_dev = inetdev_get_by_name(cJSON_GetObjectString(iface_desc, "nam"
		                                                                  "e"));
		if (!iface_dev) {
			goto outerr;
		}

		if (1
		    != inet_pton(AF_INET, cJSON_GetObjectString(iface_desc, "ip"),
		        &if_addr)) {
			goto outerr;
		}

		if (1
		    != inet_pton(AF_INET, cJSON_GetObjectString(iface_desc, "netmask"),
		        &if_netmask)) {
			goto outerr;
		}

		if (1
		    != inet_pton(AF_INET, cJSON_GetObjectString(iface_desc, "gateway"),
		        &if_gateway)) {
			goto outerr;
		}

		if (!macaddr_scan((unsigned char *)cJSON_GetObjectString(iface_desc,
		                      "mac"),
		        if_hwaddr)) {
			goto outerr;
		}
		if (netdev_set_macaddr(iface_dev->dev, if_hwaddr)) {
			goto outerr;
		}

		rt_del_route_if(iface_dev->dev);

		inetdev_set_addr(iface_dev, if_addr.s_addr);

		inetdev_set_mask(iface_dev, if_netmask.s_addr);
		rt_add_route(iface_dev->dev, if_addr.s_addr & if_netmask.s_addr,
		    if_netmask.s_addr, INADDR_ANY, 0);

		rt_add_route(iface_dev->dev, 0, 0, if_gateway.s_addr,
		    RTF_UP | RTF_GATEWAY);

#if !OPTION_GET(BOOLEAN, is_readonly)
#if USE_NETMANAGER
		cJSON *item;
		int res = 0;
		char buf[32];

		snprintf(buf, sizeof(buf), "netmanager -d %s", iface_dev->dev->name);
		res = system(buf);

		if (res < 0) {
			snprintf(buf, sizeof(buf), "netmanager -s %d %s", res,
			    iface_dev->dev->name);
		}
		else {
			item = cJSON_GetObjectItem(iface_desc, "useDhcp");
			log_debug("res(%d) item(%d)", res, item->valueint);
			snprintf(buf, sizeof(buf), "netmanager -s %d %s", item->valueint,
			    iface_dev->dev->name);
		}
		system(buf);
		log_info("\nNet configuration is successfully saved");
		log_info("\tRebooting now to apply new net config...");
		platform_shutdown(SHUTDOWN_MODE_REBOOT);
#else  /*not netmanager */
		if (!system("flash_settings store net")) {
			log_info("\nNet configuration is successfully saved");
			log_info("\tRebooting now to apply new net config...");
			platform_shutdown(SHUTDOWN_MODE_REBOOT);
		}
		else {
			log_error("\nNet configuration saving failed");
		}
#endif /* USE_NETMANAGER */
#else  /* is readonly config */
		log_info("Net configuration is updated now");
#endif /* is_readonly */
	}
	cJSON_Delete(post_json);
	return;

outerr:
	log_error("failed");
	cJSON_Delete(post_json);
}

int main(int argc, char *argv[]) {
	char *method;

	printf("HTTP/1.1 %d %s\r\n"
	       "Content-Type: %s\r\n"
	       "Connection: close\r\n"
	       "\r\n",
	    200, "OK", "application/json");

	method = getenv("REQUEST_METHOD");
	if (0 == strcmp("GET", method)) {
		char *list = http_admin_build_iface_list();
		printf("%s\n", list);
		free(list);
	}
	else if (0 == strcmp("POST", method)) {
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
