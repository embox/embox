/* $Id$ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

/**
 * simple_pjsua.c
 *
 * This is a very simple but fully featured SIP user agent, with the 
 * following capabilities:
 *  - SIP registration
 *  - Making and receiving call
 *  - Audio/media to sound device.
 *
 * Usage:
 *  - To make outgoing call, start simple_pjsua with the URL of remote
 *	destination to contact.
 *	E.g.:
 *		 simpleua sip:user@remote
 *
 *  - Incoming calls will automatically be answered with 200.
 *
 * This program will quit once it has completed a single call.
 */

#include <sys/time.h>
#include <time.h>

#include <lib/loop_file_logger.h>

#include <pjsua-lib/pjsua.h>

#include "main.h"

#include <framework/mod/options.h>

#define USE_ACC_FILE OPTION_GET(BOOLEAN, use_acc_file)

#if USE_ACC_FILE
#define ACC_FILE_NAME    OPTION_STRING_GET(acc_file_name)

static char sip_domain_buf[32];
#define SIP_DOMAIN sip_domain_buf

static char sip_user_buf[32];
#define SIP_USER sip_user_buf

static char sip_passwd_buf[32];
#define SIP_PASSWD sip_passwd_buf

static void load_account(void) {
	FILE *fp;
	char str[128];


	fp = fopen(ACC_FILE_NAME , "r");
	if (!fp) {
		printf("error during loading account file %s", ACC_FILE_NAME);
		return;
	}
	memset(str, 0, sizeof(str));
	while (fgets(str, sizeof(str), fp)) {
		if (0 == strncmp(str, "sip_domain: ", sizeof("sip_domain: ") - 1)) {
			str[strlen(str) - 1] = '\0';
			strcpy(SIP_DOMAIN, &str[sizeof("sip_domain: ") - 1]);
			memset(str, 0, sizeof(str));
			continue;
		}
		if (0 == strncmp(str, "sip_user: ", sizeof("sip_user: ") - 1)) {
			str[strlen(str) - 1] = '\0';
			strcpy(SIP_USER, &str[sizeof("sip_user: ") - 1]);
			memset(str, 0, sizeof(str));
			continue;
		}
		if (0 == strncmp(str, "sip_passwd: ", sizeof("sip_passwd: ") - 1)) {
			str[strlen(str) - 1] = '\0';
			strcpy(SIP_PASSWD, &str[sizeof("sip_passwd: ") - 1]);
			memset(str, 0, sizeof(str));
			continue;
		}
	}
}

void store_sip_account(char *domain, char *user, char *passwd) {
	char file_buf[256] = "";

	strcpy(file_buf, "sip_domain: ");
	strcat(file_buf, domain);
	strcat(file_buf, "\n");

	strcat(file_buf, "sip_user: ");
	strcat(file_buf, user);
	strcat(file_buf, "\n");

	strcat(file_buf, "sip_passwd: ");
	strcat(file_buf, passwd);
	strcat(file_buf, "\n");

	FILE *file = fopen(ACC_FILE_NAME, "w");
	if (!file) {
		printf("Couldn't write file %s...\n", ACC_FILE_NAME);

		return ;
	}
	fwrite(file_buf, strlen(file_buf), 1, file);
	fclose(file);
}
#else
#include <simple_pjsua_sip_account.inc>


static inline void load_account(void) {}
void store_sip_account(char *domain, char *user, char *passwd) {}
#endif

char *get_sip_acc_domain(void) {
	return SIP_DOMAIN;
}

char *get_sip_acc_user(void) {
	return SIP_USER;
}

char *get_sip_acc_passwd(void) {
	return SIP_PASSWD;
}

static inline char *get_sip_id_str(char buf[]) {
	strcpy(buf, "sip:");
	strcat(buf, SIP_USER);
	strcat(buf, "@");
	strcat(buf, SIP_DOMAIN);

	return buf;
}
static inline char *get_sip_uri_str(char buf[]) {
	strcpy(buf, "sip:");
	strcat(buf, SIP_DOMAIN);

	return buf;
}

#define THIS_FILE		"APP"

static pjsua_call_id current_call = PJSUA_INVALID_ID;

struct demo_call_info *call_info;

static struct timeval tv_start_call;
static char pjsip_log_message_buf[128];

/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
							 pjsip_rx_data *rdata)
{
	pjsua_call_info ci;

	PJ_UNUSED_ARG(acc_id);
	PJ_UNUSED_ARG(rdata);

	pjsua_call_get_info(call_id, &ci);

	sprintf(call_info->incoming, "%.*s",
		(int)ci.remote_info.slen, ci.remote_info.ptr);
	call_info->state = CALL_INCOMING;

	PJ_LOG(3,(THIS_FILE, call_info->incoming));

	current_call = call_id;

	/* Automatically answer incoming calls with 200/OK */
	//pjsua_call_answer(call_id, 200, NULL, NULL);

	gettimeofday(&tv_start_call, NULL);

	demo_nk_on_incoming_call();
}

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
	pjsua_call_info ci;

	PJ_UNUSED_ARG(e);

	pjsua_call_get_info(call_id, &ci);
	PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
						 (int)ci.state_text.slen,
						 ci.state_text.ptr));
	if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
		struct timeval tv_end, tv_res;
		char dur[8];
		time_t time;


		call_info->state = CALL_INACTIVE;
		current_call = PJSUA_INVALID_ID;
		//demo_pj_hang();

		memset(pjsip_log_message_buf, 0, sizeof(pjsip_log_message_buf));

		time = tv_start_call.tv_sec;
		ctime_r(&time, pjsip_log_message_buf);
		strncat(pjsip_log_message_buf, ": ", sizeof(pjsip_log_message_buf) - 1 );
		strncat(pjsip_log_message_buf, ci.remote_contact.ptr, sizeof(pjsip_log_message_buf) - 1);
		strncat(pjsip_log_message_buf, ": dur: ", sizeof(pjsip_log_message_buf) - 1 );

		gettimeofday(&tv_end, NULL);
		timersub(&tv_end, &tv_start_call, &tv_res);
		time = tv_res.tv_sec;
		itoa(time, dur, 10);

		//call_info->has_log = 1;
		strncat(pjsip_log_message_buf, dur, sizeof(pjsip_log_message_buf) - 1);
		loop_logger_write(pjsip_log_message_buf);

		demo_nk_on_incoming_call();
	}
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
	pjsua_call_info ci;

	pjsua_call_get_info(call_id, &ci);

	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
		// When media is active, connect call to sound device.
		pjsua_conf_connect(ci.conf_slot, 0);
		pjsua_conf_connect(0, ci.conf_slot);
	}
}

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
	pjsua_perror(THIS_FILE, title, status);
	pjsua_destroy();
	exit(1);
}


void demo_pj_answer(void) {
	pjsua_call_info ci;

	if (current_call != PJSUA_INVALID_ID) {
		pjsua_call_answer(current_call, 200, NULL, NULL);

		pjsua_call_get_info(current_call, &ci);

		snprintf(call_info->remote_uri, sizeof call_info->remote_uri - 1,
			"Remote URI: %s", ci.remote_info.ptr);
		snprintf(call_info->remote_contact, sizeof call_info->remote_contact - 1,
			"Remote contact: %s", ci.remote_contact.ptr);

		call_info->state = CALL_ACTIVE;
	}
}

void demo_pj_hang(void) {
	pjsua_call_hangup_all();

	current_call = PJSUA_INVALID_ID;

	call_info->state = CALL_INACTIVE;
}

/*
 * main()
 *
 * argv[1] may contain URL to call.
 */
int demo_pj_main(int argc, char *argv[], int (*nk_cb)(const char *),
	const char *input_dev_path)
{
	pjsua_acc_id acc_id;
	pj_status_t status;

	/* Create pjsua first! */
	status = pjsua_create();
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);

	/* If argument is specified, it's got to be a valid SIP URL */
	if (argc > 1) {
		status = pjsua_verify_url(argv[1]);
		if (status != PJ_SUCCESS) error_exit("Invalid URL in argv", status);
	}

	/* Init pjsua */
	{
		pjsua_config cfg;
		pjsua_logging_config log_cfg;

		pjsua_config_default(&cfg);
		cfg.cb.on_incoming_call = &on_incoming_call;
		cfg.cb.on_call_media_state = &on_call_media_state;
		cfg.cb.on_call_state = &on_call_state;

		pjsua_logging_config_default(&log_cfg);
		log_cfg.console_level = 1;

		status = pjsua_init(&cfg, &log_cfg, NULL);
		if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
	}

	/* Add UDP transport. */
	{
		pjsua_transport_config cfg;

		pjsua_transport_config_default(&cfg);
		cfg.port = 5060;
		status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
		if (status != PJ_SUCCESS) error_exit("Error creating transport", status);
	}

	/* Initialization is done, now start pjsua */
	status = pjsua_start();
	if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);

	/* Register to SIP server by creating SIP account. */
	{
		pjsua_acc_config cfg;
		static char id_buf[128];
		static char uri_buf[64];

		load_account();

		printf("sip domain: %s\n", SIP_DOMAIN);
		printf("sip user: %s\n", SIP_USER);
		printf("sip passwd: %s\n", SIP_PASSWD);

		pjsua_acc_config_default(&cfg);

		memset(id_buf, 0, sizeof(id_buf));
		cfg.id = pj_str(get_sip_id_str(id_buf));

		memset(uri_buf, 0, sizeof(uri_buf));
		cfg.reg_uri = pj_str(get_sip_uri_str(uri_buf));

		cfg.cred_count = 1;
		cfg.cred_info[0].realm = pj_str("*");
		cfg.cred_info[0].scheme = pj_str("digest");
		cfg.cred_info[0].username = pj_str(SIP_USER);
		cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
		cfg.cred_info[0].data = pj_str(SIP_PASSWD);

		status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
		if (status != PJ_SUCCESS) error_exit("Error adding account", status);
	}

	/* If URL is specified, make call to the URL. */
	if (argc > 1) {
		pj_str_t uri = pj_str(argv[1]);
		status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
		if (status != PJ_SUCCESS) error_exit("Error making call", status);
	}

	nk_cb(input_dev_path);

	/* Wait until user press "q" to quit. */
	for (;;) {
		char option[10];

		puts("Press 'a' to answer call, 'h' to hangup all calls, 'q' to quit");
		if (fgets(option, sizeof(option), stdin) == NULL) {
			puts("EOF while reading stdin, will quit now..");
			break;
		}

		if (option[0] == 'q')
			break;

		if (option[0] == 'h')
			pjsua_call_hangup_all();

		if (option[0] == 'a') {
			demo_pj_answer();
		}
	}

	/* Destroy pjsua */
	pjsua_destroy();

	return 0;
}
