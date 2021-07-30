/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.07.2014
 */
#include <util/log.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>


#include <cJSON.h>
#include <embox/cmd.h>
#include <kernel/printk.h>
#include <hal/arch.h>

#include <simple_pjsua_sip_account.inc>

static char *http_admin_build_accounts_list(void) {
	cJSON *accounts_array;
	char *json_list;

	accounts_array = cJSON_CreateArray();
	if (!accounts_array) {
		log_error("could not create cJSON_CreateArray()");
		goto outerr;
	}

	cJSON *account_obj;

	account_obj = cJSON_CreateObject();
	if (!account_obj) {
		log_error("could not create cJSON_CreateObject()");
		goto outerr;
	}
	cJSON_AddItemToArray(accounts_array, account_obj);

		/* FIXME cJSON_Add* are not report failed memory allocation */

	cJSON_AddStringToObject(account_obj, "name", "main account");

	cJSON_AddStringToObject(account_obj, "sip_domain", SIP_DOMAIN);

	cJSON_AddStringToObject(account_obj, "sip_user", SIP_USER);

	cJSON_AddStringToObject(account_obj, "sip_password", SIP_PASSWD);

	json_list = cJSON_PrintUnformatted(accounts_array);

	cJSON_Delete(accounts_array);

	return json_list;
outerr:
	cJSON_Delete(accounts_array);
	return strdup("{}");
}

static void http_admin_post(char *post_data) {
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
		char *list = http_admin_build_accounts_list();
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
