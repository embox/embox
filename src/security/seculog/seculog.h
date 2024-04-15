/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.09.2013
 */

#ifndef SECURITY_SECULOG_H_
#define SECURITY_SECULOG_H_

#include <stdint.h>
#include <lib/libds/dlist.h>

#define SECULOG_LABEL_MANDATORY (1 << 0)
#define SECULOG_LABEL_LOGIN_ACT (1 << 1)

typedef uint32_t seculog_label_t;

/**
 * @brief Record activity in security log.
 *
 * @param label Label to assign the message, should be one of SECULOG_LABEL_*
 * @param msg Message to log
 *
 * @return
 */
extern int seculog_record(seculog_label_t label, const char *msg);

#define SECULOG_RECORD_MAX_MSG_N 256

struct seculog_record {
	seculog_label_t label;
	char msg[SECULOG_RECORD_MAX_MSG_N + 1];

	int refcount;
};

struct seculog_subscb;

typedef int (*seculog_record_cb_t)(struct seculog_subscb *subcb,
		struct seculog_record *record);

struct seculog_subscb {
	/* seculog private */
	struct dlist_head lnk;

	/* filled by subscriber */
	seculog_label_t labels;
	seculog_record_cb_t record_cb;
};

struct seculog_desc {
	struct seculog_subscb subscb;
	struct dlist_head queue;
};

/**
 * @brief Subscribe to updates in security log.
 *
 * @param subscb Subscriber defined structure
 *
 * @return
 */
extern int seculog_subscribe(struct seculog_subscb *subscb);

/**
 * @brief Open security log for sepecified labels
 *
 * @param labels
 * @param desc
 *
 * @return
 */
extern int seculog_open(seculog_label_t labels, struct seculog_desc *desc);

/**
 * @brief Get next message from opened \a seculog_desc
 *
 * @param desc
 * @param msg
 * @param len
 *
 * @return
 */
extern int seculog_get(struct seculog_desc *desc, char *msg, size_t len);

/**
 * @brief Close seculog_desc
 *
 * @param desc
 *
 * @return
 */
extern int seculog_close(struct seculog_desc *desc);

#endif /* SECURITY_SECULOG_H_ */
