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

typedef uint32_t seculog_label_t;
typedef int (*seculog_record_cb_t)(seculog_label_t label, const char *msg);

#define SECULOG_LABEL_MANDATORY (1 << 0)
#define SECULOG_LABEL_LOGIN_ACT (1 << 1)

struct seculog_subscb {
	/* seculog private */
	struct dlist_head lnk;

	/* filled by subscriber */
	seculog_label_t labels;
	seculog_record_cb_t record_cb;
};

/**
 * @brief Record activity in security log.
 *
 * @param label Label to assign the message, should be one of SECULOG_LABEL_*
 * @param msg Message to log
 *
 * @return
 */
extern int seculog_record(seculog_label_t label, const char *msg);

/**
 * @brief Subscribe to updates in security log.
 *
 * @param subscb Subscriber defined structure
 *
 * @return
 */
extern int seculog_subscribe(struct seculog_subscb *subscb);

#endif /* SECURITY_SECULOG_H_ */
