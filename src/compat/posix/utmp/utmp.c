/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.02.2013
 */

#include <string.h>
#include <errno.h>
#include <utmp.h>

#include <framework/mod/options.h>

#define MAX_ENT_COUNT OPTION_GET(NUMBER, max_ent_count)

static int utmp_f_pos, utmp_f_end;
static struct utmp utmp_f[MAX_ENT_COUNT];
static struct utmp result;

static struct utmp *ret_result(struct utmp *ut) {
	return memcpy(&result, ut, sizeof(struct utmp));
}

struct utmp *getutent(void) {
	if (utmp_f_pos >= utmp_f_end) {
		errno = EEOF;
		return NULL;
	}

	return ret_result(&utmp_f[utmp_f_pos++]);
}

struct utmp *getutid(struct utmp *ut) {
	int mode;
	if (ut->ut_type == RUN_LVL
			|| ut->ut_type == BOOT_TIME
			|| ut->ut_type == NEW_TIME
			|| ut->ut_type == OLD_TIME) {
		mode = 0;
	} else if (ut->ut_type == INIT_PROCESS
			|| ut->ut_type == LOGIN_PROCESS
			|| ut->ut_type == USER_PROCESS
			|| ut->ut_type == DEAD_PROCESS) {
		mode = 1;
	}

	while (utmp_f_pos < utmp_f_end) {
		if ((!mode && ut->ut_type == utmp_f[utmp_f_pos].ut_type) ||
				(mode && 0 == strcmp(ut->ut_id, utmp_f[utmp_f_pos].ut_id))) {
			return ret_result(&utmp_f[utmp_f_pos++]);
		}
		utmp_f_pos++;
	}

	errno = ENOENT;
	return NULL;
}

struct utmp *getutline(struct utmp *ut) {
	while (utmp_f_pos < utmp_f_end) {
		short utt = utmp_f[utmp_f_pos].ut_type;

		if ((utt == USER_PROCESS || utt == LOGIN_PROCESS) &&
			0 == strcmp(utmp_f[utmp_f_pos].ut_line, ut->ut_line)) {
			return ret_result(&utmp_f[utmp_f_pos++]);
		}

		utmp_f_pos++;
	}

	errno = ENOENT;
	return NULL;
}

struct utmp *pututline(struct utmp *ut) {
	struct utmp *rec;

	setutent();

	rec = getutid(ut);

	if (NULL == rec) {
		if (utmp_f_end >= MAX_ENT_COUNT) {
			errno = ENOMEM;
			return NULL;
		}

		utmp_f_pos = ++utmp_f_end;
	}

	rec = &utmp_f[--utmp_f_pos];
	++utmp_f_pos;

	return memcpy(rec, ut, sizeof(struct utmp));

}

void setutent(void) {
	utmp_f_pos = 0;
}

void endutent(void) {
}

int utmpname(const char *file) {
	errno = ENOMEM;
	return -1;
}

