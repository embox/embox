/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.09.2013
 */

#include <util/dlist.h>

#include <security/seculog.h>

static DLIST_DEFINE(seculog_subscb_list);

int seculog_record(seculog_label_t label, const char *msg) {
	struct seculog_subscb *subscb, *nxt;

	dlist_foreach_entry(subscb, nxt, &seculog_subscb_list, lnk) {
		if (label & subscb->labels) {
			if (subscb->record_cb) {
				subscb->record_cb(label, msg);
			}
		}
	}

	return 0;
}

int seculog_subscribe(struct seculog_subscb *subscb) {

	dlist_head_init(&subscb->lnk);
	dlist_add_next(&subscb->lnk, &seculog_subscb_list);

	return 0;
}

#if 0
int seculog_open(struct seculog_desc *desc) {

}

int seculog_get(struct seculog_desc *desc, struct seculog_record *rec) {

}

int seculog_next(struct seculog_desc *desc, seculog_label_t labels, unsigned int skip) {

}
#endif

