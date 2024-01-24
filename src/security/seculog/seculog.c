/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    03.09.2013
 */

#include <errno.h>
#include <string.h>
#include <mem/misc/pool.h>
#include <lib/libds/dlist.h>

#include <security/seculog/seculog.h>

#define RECORDS_N 32
#define REC_HND_N 32

struct seculog_rec_handle {
	struct dlist_head rh_lnk;
	struct seculog_record *record;
};

static DLIST_DEFINE(seculog_subscb_list);
POOL_DEF(seculog_record_pool, struct seculog_record, RECORDS_N);
POOL_DEF(seculog_rec_handle_pool, struct seculog_rec_handle, REC_HND_N);

static struct seculog_record *seculog_record_alloc(seculog_label_t label,
		const char *msg) {
	struct seculog_record *rec;

	if (NULL == (rec = pool_alloc(&seculog_record_pool))) {
		return NULL;
	}

	rec->label = label;
	rec->refcount = 1;
	strncpy(rec->msg, msg, SECULOG_RECORD_MAX_MSG_N);

	return rec;
}

static void seculog_record_free(struct seculog_record *rec) {

	assert(rec->refcount > 0);

	if (--rec->refcount == 0) {
		pool_free(&seculog_record_pool, rec);
	}
}

static void seculog_record_inc_ref(struct seculog_record *rec) {
	++rec->refcount;
}

int seculog_record(seculog_label_t label, const char *msg) {
	struct seculog_subscb *subscb;

	struct seculog_record *rec = seculog_record_alloc(label, msg);

	if (!rec) {
		return -ENOMEM;
	}

	dlist_foreach_entry(subscb, &seculog_subscb_list, lnk) {
		if (label & subscb->labels) {
			if (subscb->record_cb) {
				subscb->record_cb(subscb, rec);
			}
		}
	}

	seculog_record_free(rec);

	return 0;
}

int seculog_subscribe(struct seculog_subscb *subscb) {

	dlist_head_init(&subscb->lnk);
	dlist_add_next(&subscb->lnk, &seculog_subscb_list);

	return 0;
}

int seculog_unsubscribe(struct seculog_subscb *subscb) {

	dlist_del(&subscb->lnk);

	return 0;
}

static struct seculog_rec_handle *seculog_rec_handle_alloc(struct seculog_record *rec) {
	struct seculog_rec_handle *rech;

	if (NULL != (rech = pool_alloc(&seculog_rec_handle_pool))) {
//		return rech;


	dlist_head_init(&rech->rh_lnk);
	rech->record = rec;
	seculog_record_inc_ref(rec);
	}

	return rech;
}

static void seculog_rec_handle_free(struct seculog_rec_handle *rech) {

	seculog_record_free(rech->record);

	pool_free(&seculog_rec_handle_pool, rech);
}

static int seculog_cb_opend(struct seculog_subscb *subscb, struct seculog_record *rec) {
	struct seculog_desc *desc = (struct seculog_desc *) subscb;
	struct seculog_rec_handle *rech;

	if (!(rech = seculog_rec_handle_alloc(rec))) {
		return -ENOMEM;
	}

	dlist_add_prev(&rech->rh_lnk, &desc->queue);

	return 0;
}

int seculog_open(seculog_label_t labels, struct seculog_desc *desc) {
	struct seculog_subscb *scb = &desc->subscb;

	scb->labels = labels;
	scb->record_cb = seculog_cb_opend;

	dlist_init(&desc->queue);

	return seculog_subscribe(scb);
}

int seculog_get(struct seculog_desc *desc, char *msg, size_t len) {
	struct seculog_rec_handle *rech;

	if (dlist_empty(&desc->queue)) {
		return -EAGAIN;
	}

	rech = (struct seculog_rec_handle *) desc->queue.next;

	dlist_del(&rech->rh_lnk);

	strncpy(msg, rech->record->msg, len);

	seculog_rec_handle_free(rech);

	return 0;
}

int seculog_close(struct seculog_desc *desc) {
	struct seculog_rec_handle *rech;

	dlist_foreach_entry(rech, &desc->queue, rh_lnk) {
		seculog_rec_handle_free(rech);
	}

	return seculog_unsubscribe(&desc->subscb);
}

