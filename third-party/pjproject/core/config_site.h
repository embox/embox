#define PJ_LOG_USE_STACK_BUFFER        0

#define PJ_LOG_MAX_LEVEL 6

#define PJ_POOL_DEBUG        0
#define PJ_HAS_POOL_ALT_API  0

/* make PJSUA slim */
#define PJSUA_MAX_ACC 3
#define PJSUA_MAX_CALLS 1
#define PJSUA_MAX_VID_WINS 0
#define PJSUA_MAX_BUDDIES 1
#define PJSUA_MAX_CONF_PORTS 4
#define PJSUA_MAX_PLAYERS 1
#define PJSUA_MAX_RECORDERS 1

/* Changing to #if 0 will increase memory consumption
 * but insreases communication speed. */
#if 1
	/* This sample derived from pjlib/include/pj/config_site_sample.h: */
	#define PJ_OS_HAS_CHECK_STACK    0
	#define PJ_ENABLE_EXTRA_CHECK    0
	#define PJ_HAS_ERROR_STRING      0
	#undef PJ_IOQUEUE_MAX_HANDLES
	#define PJ_IOQUEUE_MAX_HANDLES   8
	#define PJ_CRC32_HAS_TABLES      0
	#define PJSIP_MAX_TSX_COUNT      15
	#define PJSIP_MAX_DIALOG_COUNT   15
	#define PJSIP_UDP_SO_SNDBUF_SIZE 4000
	#define PJSIP_UDP_SO_RCVBUF_SIZE 4000
	#define PJMEDIA_HAS_ALAW_ULAW_TABLE  0
#endif
