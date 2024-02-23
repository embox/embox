/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    10.06.2014
 */

#include <unistd.h>
#include <fcntl.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <kernel/task/resource/idesc.h>
#include <fs/vfs.h>
#include <security/seculog/seculog.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(seculog_file_init);

#define SMAC_AUDIT_FILE OPTION_STRING_GET(audit_file)

static struct file_desc *audit_log_desc;

static int audit_log_open(struct file_desc **log_desc_p) {
	struct file_desc *log_desc;
	int fd;

	fd = creat(SMAC_AUDIT_FILE, 0777);
	if (fd < 0) {
		return -1;
	}

	if (!(log_desc = file_desc_get(fd))) {
		close(fd);
		return -1;
	}

	/* This is `forever' file_desc, prevent it from to be free */
	log_desc->f_idesc.idesc_usage_count++;
	close(fd);

	*log_desc_p = log_desc;

	return 0;
}

static int seculog_file_cb(struct seculog_subscb *subcb, struct seculog_record *record) {
	static char no_audit;

	if (no_audit) {
		return 0;
	}

	if (!audit_log_desc) {
		int ret;
		no_audit = 1;
		ret = audit_log_open(&audit_log_desc);
		no_audit = 0;

		if (ret) {
			return 0;
		}
	}

	kwrite(audit_log_desc, record->msg, strlen(record->msg));

	return 0;
}

struct seculog_subscb seculog_file_subscb = {
	.labels = SECULOG_LABEL_MANDATORY | SECULOG_LABEL_LOGIN_ACT,
	.record_cb = seculog_file_cb,
};

static int seculog_file_init(void) {
	return seculog_subscribe(&seculog_file_subscb);
}
