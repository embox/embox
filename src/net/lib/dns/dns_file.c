/**
 * @file
 * @brief
 *
 * @date 22.08.12
 * @author Ilia Vaprol
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <util/err.h>

#include <net/lib/dns.h>

#include <framework/mod/options.h>

/**
 * DNS nameservers
 */
#define NAMESERVER_DEFAULT OPTION_STRING_GET(nameserver)
#define RESOLV_FILE        OPTION_STRING_GET(resolv_file)

static char nameserver_ip[16]; /* only for IPv4 */

const char *dns_get_nameserver(void) {
	return nameserver_ip;
}

char *dns_set_nameserver(char *nameserver) {
	FILE *file;
	char buf[0x40];
	char *res;

	file = fopen(RESOLV_FILE, "w+");
	if (file == NULL) {
		return err_ptr(errno);
	}
	while(NULL != (res = fgets(buf, sizeof(buf), file))) {
		if (0 == strncmp(buf, "nameserver ", 11)) {
			int i;
			int len;

			/* remove line */
			len = strlen(buf);
			for(i = 0; i < len; i ++) {
				fwrite("\b", 1, 1, file);
			}

			fwrite(buf, 1, strlen(buf), file);

			strncpy(nameserver_ip, nameserver, sizeof(nameserver_ip)-1);
			nameserver_ip[sizeof(nameserver_ip) - 1] = '\0';

			fclose(file);

			return nameserver_ip;
		}
	}
	sprintf(buf, "nameserver %s\n", nameserver);
	fwrite(buf, 1, strlen(buf), file);

	strncpy(nameserver_ip, nameserver, sizeof(nameserver_ip)-1);
	nameserver_ip[sizeof(nameserver_ip) - 1] = '\0';

	fclose(file);

	return nameserver_ip;
}

char *dns_init_nameserver(char *nameserver) {
	char *res;

	res = dns_set_nameserver(nameserver);

	return res;
}
