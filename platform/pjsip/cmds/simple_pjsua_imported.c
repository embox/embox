/**
 * @file
 *
 * Call and accept calls with account registration
 *
 * @date 26.10.2018
 * @author Alexander Kalmuk
 */

#include <pjsua-lib/pjsua.h>

#include <framework/mod/options.h>

#include <mem/heap/mspace_malloc.h>

#define THIS_FILE	"APP"

#define SIP_DOMAIN	OPTION_STRING_GET(sip_domain)
#define SIP_USER	OPTION_STRING_GET(sip_user)
#define SIP_PASSWD	OPTION_STRING_GET(sip_passwd)

#define PJ_MAX_PORTS 16

#if OPTION_GET(BOOLEAN,use_extern_mem)
#define MM_SET_HEAP(type, p_prev_type) \
	mspace_set_heap(type, p_prev_type)
#else
#define MM_SET_HEAP(type, prev_type) \
	(void) prev_type
#endif

static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
				pjsip_rx_data *rdata) {
	pjsua_call_info ci;

	pjsua_call_get_info(call_id, &ci);

	PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
			(int)ci.remote_info.slen,
			ci.remote_info.ptr));

	/* Automatically answer incoming calls with 200/OK */
	{
		heap_type_t prev_type;
		MM_SET_HEAP(HEAP_RAM, &prev_type);
		pjsua_call_answer(call_id, 200, NULL, NULL);
		MM_SET_HEAP(prev_type, NULL);
	}
}

static void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
	pjsua_call_info ci;

	PJ_UNUSED_ARG(e);

	pjsua_call_get_info(call_id, &ci);
	PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
			 (int)ci.state_text.slen,
			 ci.state_text.ptr));
}

static void print_available_conf_ports(void) {
	pjsua_conf_port_id id[PJ_MAX_PORTS];
	unsigned port_cnt;
	pj_status_t status;
	int i;

	status = pjsua_enum_conf_ports(id, &port_cnt);
	assert(status == PJ_SUCCESS);

	printf("\nAvailable conference audio port count: %d\n", port_cnt);

	for (i = 0; i < port_cnt; i++) {
		pjsua_conf_port_info info;
		pjsua_conf_get_port_info(id[i], &info);
		assert(status == PJ_SUCCESS);
		printf("Port(%d): name=%s\n", i, pj_strbuf(&info.name));
	}
	printf("\n");
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id) {
	pjsua_call_info ci;

	pjsua_call_get_info(call_id, &ci);

	print_available_conf_ports();

	if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
		// When media is active, connect call to sound device.
		pjsua_conf_connect(ci.conf_slot, 0);
		pjsua_conf_connect(0, ci.conf_slot);
	}
}

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status) {
	pjsua_perror(THIS_FILE, title, status);
	pjsua_destroy();
	MM_SET_HEAP(HEAP_RAM, NULL);
	exit(1);
}

static void init_pjsua(void) {
	pj_status_t status;
	pjsua_config cfg;
	pjsua_logging_config log_cfg;

	pjsua_config_default(&cfg);
	cfg.cb.on_incoming_call = &on_incoming_call;
	cfg.cb.on_call_media_state = &on_call_media_state;
	cfg.cb.on_call_state = &on_call_state;

	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 4;

	status = pjsua_init(&cfg, &log_cfg, NULL);
	if (status != PJ_SUCCESS) {
		error_exit("pjsua_init() failed", status);
	}
}

static void init_udp_transport(void) {
	pjsua_transport_config cfg;
	pj_status_t status;

	pjsua_transport_config_default(&cfg);
	cfg.port = 5060;
	status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
	if (status != PJ_SUCCESS) {
		error_exit("Error creating transport", status);
	}
}

static void register_acc(pjsua_acc_id *acc_id) {
	pjsua_acc_config cfg;
	pj_status_t status;

	pjsua_acc_config_default(&cfg);
	cfg.id = pj_str("sip:" SIP_USER "@" SIP_DOMAIN);
	cfg.reg_uri = pj_str("sip:" SIP_DOMAIN);
	cfg.cred_count = 1;
	cfg.cred_info[0].realm = pj_str("*");
	cfg.cred_info[0].scheme = pj_str("digest");
	cfg.cred_info[0].username = pj_str(SIP_USER);
	cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cfg.cred_info[0].data = pj_str(SIP_PASSWD);

	status = pjsua_acc_add(&cfg, PJ_TRUE, acc_id);
	if (status != PJ_SUCCESS) {
		error_exit("Error adding account", status);
	}
}

/*
 * main()
 *
 * argv[1] may contain URL to call.
 */
int main(int argc, char *argv[]) {
	pjsua_acc_id acc_id;
	pj_status_t status;

	MM_SET_HEAP(HEAP_EXTERN_MEM, NULL);

	status = pjsua_create();
	if (status != PJ_SUCCESS) {
		error_exit("pjsua_create() failed", status);
	}

	init_pjsua();
	init_udp_transport();

	status = pjsua_start();
	if (status != PJ_SUCCESS) {
		error_exit("Error starting pjsua", status);
	}

	register_acc(&acc_id);

	/* If URL is specified, make call to the URL. */
	if (argc > 1) {
		pj_str_t uri = pj_str(argv[1]);

		{
			heap_type_t prev_type;
			MM_SET_HEAP(HEAP_RAM, &prev_type);
			status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, NULL);
			MM_SET_HEAP(prev_type, NULL);
		}

		if (status != PJ_SUCCESS) {
			error_exit("Error making call", status);
		}
	}

	/* Wait until user press "q" to quit. */
	for (;;) {
		char option[10];

		puts("Press 'h' to hangup all calls, 'q' to quit");
		if (fgets(option, sizeof(option), stdin) == NULL) {
			puts("EOF while reading stdin, will quit now..");
			break;
		}

		if (option[0] == 'q')
			break;

		if (option[0] == 'h')
			pjsua_call_hangup_all();
	}

	/* Destroy pjsua */
	pjsua_destroy();
	MM_SET_HEAP(HEAP_RAM, NULL);

	return 0;
}
