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
#include <util/log.h>

#include <net/lib/dns.h>

#include <framework/mod/options.h>

#define RESOLV_FILE        OPTION_STRING_GET(resolv_file)

static int nameserver_inited = 0;
static char nameserver_ip[16]; /* only for IPv4 */

const char *dns_get_nameserver(void) {
	char buf[128];
	FILE *input;
	int ret;
	char *res = NULL;
	unsigned char ip4_addr[sizeof(struct in_addr)];

	if (nameserver_inited) {
		return nameserver_ip;
	}

	/* Read resolv.conf only once and store obtained IP address
	 * to nameserver_ip */
	nameserver_inited = 1;

	input = fopen(RESOLV_FILE, "r");
	if (!input) {
		log_error("No file %s", RESOLV_FILE);
		return NULL;
	}

	/* Skip comments */
	while ((ret = fscanf(input, "%s", buf)) != EOF
		    && !strncmp(buf, "#", 1)) {
		char c;

		do {
			c = fgetc(input);
		} while (c != '\n' && c != EOF);
	}

	if (ret == EOF) {
		goto out_close;
	}

	if (strcmp(buf, "nameserver")) {
		log_error("Unknown keyword: %s", buf);
		goto out_close;
	}

	fscanf(input, "%s", buf);

	if (1 != inet_pton(AF_INET, buf, &ip4_addr)) {
		log_error("wrong nameserver %s", buf);
		goto out_close;
	}

	memcpy(nameserver_ip, buf, sizeof nameserver_ip);

	res = nameserver_ip;

out_close:
	fclose(input);

	log_debug("nameserver = %s", res ? res : "unresolved");

	return (const char *) res;
}

char *dns_set_nameserver(char *nameserver) {
/* FIXME */
#if 0
	FILE *file;
	char buf[0x40];
	char *res;

	file = fopen(RESOLV_FILE, "w+");
	if (file == NULL) {
		return err2ptr(errno);
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
#endif
	return NULL;
}

char *dns_init_nameserver(char *nameserver) {
	char *res;

	res = dns_set_nameserver(nameserver);

	return res;
}
