/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2014
 */

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include <embox/unit.h>

#include <pwd_db.h>

EMBOX_UNIT_INIT(init);

static int copy(const char* from, const char* to) {
	FILE *fromf, *tof;
	int a;

	if (NULL == (fromf = fopen(from, "r"))) {
		return errno;
	}

	if (NULL == (tof = fopen(to, "w"))) {
		fclose(fromf);
		return errno;
	}

	while ((a = fgetc(fromf)) != EOF) {
		fputc(a, tof);
	}

	fclose(fromf);
	fclose(tof);

	return 0;
}

static int init(void) {
	int ret;

	if (0 != (ret = copy("/passwd", PASSWD_FILE))) {
		return ret;
	}

	if (0 != (ret = copy("/group", GROUP_FILE))) {
		return ret;
	}

	if (0 > (ret = creat(SHADOW_FILE, S_IRWXU | S_IRWXG))) {
		return ret;
	}

	if (0 != (ret = copy("/shadow", SHADOW_FILE))) {
		return ret;
	}

	if (0 != (ret = copy("/adduser.conf", ADDUSER_FILE))) {
		return ret;
	}

	return 0;
}

