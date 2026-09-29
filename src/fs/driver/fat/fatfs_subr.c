/**
 * @file
 * @brief
 *
 * @date 14.08.2012
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include <fs/hlpr_path.h>
#include <fs/fat.h>

/* Weak: without embox.compat.posix.time.clock, entries carry the FAT epoch. */
extern int clock_gettime(clockid_t clk_id, struct timespec *tp)
    __attribute__((weak));

int fat_check_filename(char *filename) {
	char *point;
	int len;
	int extlen;
	/* filename.ext <= 8 + 3 + dot */
	if(MSDOS_NAME + 1 < (len = strlen(filename))) {
		return ENAMETOOLONG;
	}
	point = filename + len;
	extlen = 0;

	/* set point to a dot */
	do {
		if(*point == '.') {
			break;
		}
		point --;
		extlen++;

	} while (point > filename);

	if(*point == '.') {
		if(extlen > 4) {
			/* normal only if name is .filename */
			if(point != filename) {
				return EINVAL;
			}
		} else {
			if(len - extlen > 8) {
				return ENAMETOOLONG;
			}
			if(1 >= extlen) {
				return EINVAL;
			}
		}
	} else if(len > 8) {
		return ENAMETOOLONG;
	}
	return 0;
}
#if 0
void fat_get_filename(char *tmppath, char *filename) {
	char *p;

	p = tmppath;
	/* strip leading path separators */
	while (*tmppath == DIR_SEPARATOR) {
		strcpy((char *) tmppath, (char *) tmppath + 1);
	}
	while (*(p++));
	p--;
	while (p > tmppath && *p != DIR_SEPARATOR) {
		p--;
	}

	if (*p == DIR_SEPARATOR) {
		p++;
	}

	return;
}
#endif
/* Days since 1970-01-01 to a civil date (Howard Hinnant's days_from_civil,
 * inverted), without struct tm and so without the libc time module. */
static void fat_civil_from_days(int64_t z, int *y, unsigned *m, unsigned *d) {
	int64_t era, yoe, doy, mp;

	z += 719468;
	era = (z >= 0 ? z : z - 146096) / 146097;
	doy = z - era * 146097;                                   /* doe first */
	yoe = (doy - doy / 1460 + doy / 36524 - doy / 146096) / 365;
	*y = (int)(yoe + era * 400);
	doy = doy - (365 * yoe + yoe / 4 - yoe / 100);
	mp = (5 * doy + 2) / 153;
	*d = (unsigned)(doy - (153 * mp + 2) / 5 + 1);
	*m = (unsigned)(mp < 10 ? mp + 3 : mp - 9);
	*y += (*m <= 2);
}

/* The civil date of days since 1970-01-01, the other way (days_from_civil). */
static int64_t fat_days_from_civil(int y, unsigned m, unsigned d) {
	int64_t era, yoe, doy, doe;

	y -= m <= 2;
	era = (y >= 0 ? y : y - 399) / 400;
	yoe = y - era * 400;
	doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
	doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
	return era * 146097 + doe - 719468;
}

/* SECS since 1970 as a FAT date and time, in two-second units. Anything
 * outside 1980..2107, such as an unset clock counting from 1970, becomes the
 * FAT epoch 1980-01-01 00:00. */
static void fat_time_encode(int64_t secs, uint16_t *date, uint16_t *time) {
	int64_t days = secs / 86400;
	int64_t rem = secs % 86400;
	unsigned m, d;
	int y;

	*date = (1 << 5) | 1;
	*time = 0;

	if (secs < 0) {
		return;
	}
	fat_civil_from_days(days, &y, &m, &d);
	if (y >= 1980 && y <= 2107) {
		*date = (uint16_t)(((y - 1980) << 9) | (m << 5) | d);
		*time = (uint16_t)(((rem / 3600) << 11) | (((rem / 60) % 60) << 5)
		                   | ((rem % 60) / 2));
	}
}

static int64_t fat_time_decode(uint16_t date, uint16_t time) {
	unsigned m = (date >> 5) & 0xf, d = date & 0x1f;

	if (m < 1 || m > 12 || d < 1) {
		/* A zero date is "not recorded"; read it as the FAT epoch. */
		m = d = 1;
	}
	return fat_days_from_civil(1980 + (date >> 9), m, d) * 86400
	       + (time >> 11) * 3600 + ((time >> 5) & 0x3f) * 60
	       + (time & 0x1f) * 2;
}

static int64_t fat_now(void) {
	struct timespec ts;

	if (!clock_gettime || clock_gettime(CLOCK_REALTIME, &ts) != 0) {
		return -1;
	}
	return ts.tv_sec;
}

/* Stamps DE with the wall clock (see fat_time_encode() for a clock that is not
 * set). */
void fat_set_filetime(struct fat_dirent *de) {
	int64_t now = fat_now();
	uint16_t date, time;

	fat_time_encode(now, &date, &time);

	de->crttime_l = time & 0xff;
	de->crttime_h = time >> 8;
	de->crtdate_l = date & 0xff;
	de->crtdate_h = date >> 8;
	fat_set_wrttime(de, now);
}

/* The last-write time, and the access date with it: nothing records reads. */
void fat_set_wrttime(struct fat_dirent *de, int64_t secs) {
	uint16_t date, time;

	fat_time_encode(secs, &date, &time);

	de->lstaccdate_l = date & 0xff;
	de->lstaccdate_h = date >> 8;
	de->wrttime_l = time & 0xff;
	de->wrttime_h = time >> 8;
	de->wrtdate_l = date & 0xff;
	de->wrtdate_h = date >> 8;
}

/* What the time just written reads back as: rounded down to two seconds. */
int64_t fat_wrttime_now(void) {
	struct fat_dirent de;

	fat_set_wrttime(&de, fat_now());
	return fat_direntry_get_mtime(&de);
}

int64_t fat_direntry_get_mtime(const struct fat_dirent *de) {
	return fat_time_decode(de->wrtdate_l | (de->wrtdate_h << 8),
	    de->wrttime_l | (de->wrttime_h << 8));
}

int64_t fat_direntry_get_ctime(const struct fat_dirent *de) {
	return fat_time_decode(de->crtdate_l | (de->crtdate_h << 8),
	    de->crttime_l | (de->crttime_h << 8));
}

/*
 *	Convert a filename element from canonical (8.3) to directory entry (11)
 *	form src must point to the first non-separator character.
 *	dest must point to a 12-byte buffer.
 */
char *path_canonical_to_dir(char *dest, char *src) {

	memset(dest, (int)' ', MSDOS_NAME);
	dest[MSDOS_NAME] = 0;

	for (int i = 0; i <= 11; i++) {
		if (!*src) {
			break;
		}
		if (*src == '/') {
			break;
		}
		if (*src == '.') {
			i = 7;
			src++;
			continue;
		}
		if (*src >= 'a' && *src <='z') {
			*(dest + i) = (*src - 'a') + 'A';
		} else {
			*(dest + i) = *src;
		}
		src++;
	}

	return dest;
}

/*
 *	Convert a filename element from directory entry (11) to canonical (8.3)
 */
char *path_dir_to_canonical(char *dest, char *src, char dir) {
        int i;
        char *dst;

        dst = dest;
        memset(dest, 0, MSDOS_NAME + 2);
        for (i = 0; i < 8; i++) {
			if (*src != ' ') {
				*dst = *src;
				if (*dst >= 'A' && *dst <='Z') {
					*dst = (*dst - 'A') + 'a';
				}
				dst++;
			}
			src++;
        }
        if ((*src != ' ') && (0 == dir)) {
        	*dst++ = '.';
        }
        for (i = 0; i < 3; i++) {
			if (*src != ' ') {
				*dst = *src;
				if (*dst >= 'A' && *dst <='Z') {
					*dst = (*dst - 'A') + 'a';
				}
				dst++;
			}
			src++;
        }
        return dest;
}


