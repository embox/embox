/**
 * @file strptime.c
 * @brief writing linux function strptime for embox os
 *
 * author Tamara Deryugina <tamara.deryugina@gmail.com>
 * @date 02.03.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

static int num_length(const char *s) {
	int count = 0;
	while ((s[count] >= '0') && (s[count] <= '9')) {
		count++;
	}
	return count;
}

static int word_length(const char *s) {
	int count = 0;
	while (((s[count] >= 'A') && (s[count] <= 'Z')) || ((s[count] >= 'a') && (s[count] <= 'z'))) {
		count++;
	}
	return count;
}

static int cmp_before_percent(const char *s, const char *format) {
	int count = 0;
	while (format[count] != '\0') {
		if (format[count] == '%') {
			return count;
		}
		if (strncmp(&format[count], &s[count], 1) != 0) {
			return 0;
		}
		count++;
	}
	return count;
}

static int cmp_month_weekday_name(const char *s, const char *format, struct tm *tm, int w_leng) {
	int count = 0;
	int month = 0;
	int wday = 0;
	char arr_day[7][10] = {"sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};
	char arr_month[13][10] = {"january", "february", "march", "april", "may", "june",
		"july", "august", "september", "october", "november", "december"};

	if ((format[0] == 'a') || (format[0] == 'A')) {
		for (wday = 0; wday != 7; wday++) {
			for (count = 0; count != w_leng; count++) {
				if (tolower(s[count]) != arr_day[wday][count]) {
					break;
				}
			}
			if (count == w_leng) {
				tm->tm_wday = wday;
				return count;
			}
		}
		return 0;

	} else if ((format[0] == 'b') || (format[0] == 'B') || (format[0] == 'h')) {
		for (month = 0; month != 12; month++) {
			for (count = 0; count != w_leng; count++) {
				if (tolower(s[count]) != arr_month[month][count]) {
					break;
				}
			}
			if (count == w_leng) {
				tm->tm_mon = month;
				return count;
			}
		}
		return 0;
	}
	return 0;
}

static int def_format(const char *format, const char *s, struct tm *tm) {
	int n_leng = num_length(s);
	int w_leng = word_length(s);
	int date, res, res_equi;
	int count_equi = 0;

	date = 0;

	switch (*format) {
		case 'm':
		case 'w':
		case 'y':
		case 'Y':
		case 'H':
		case 'I':
		case 'M':
		case 'S':
		case 'j':
		case 'd':
		case 'e':
		case 'C':
		case 'U':
		case 'W':
			if (n_leng == 0) {
				return 0;
			}
			date = atoi(s);
			break;
	}

	switch (*format) {
		case 'a':
		case 'A':
		case 'b':
		case 'B':
		case 'h':
			if ((w_leng < 3) || (w_leng > 9)) {
				return 0;
			}
			res = cmp_month_weekday_name(s, format, tm, w_leng);
			if (res == 0) {
				return 0;
			}
			return res;

		case 'm':
			if ((date > 12) || (date == 0)) {
				return 0;
			}
			tm->tm_mon = date - 1;
			return n_leng;

		case 'w':
			if (date > 6) {
				return 0;
			}
			tm->tm_wday = date;
			return n_leng;

		case 'y':
			if (date > 99) {
				return 0;
			}
			if ((date > 68) && (date < 100)) {
				tm->tm_year = date;
			} else {
				tm->tm_year = date + 100;
			}
			return n_leng;

		case 'Y':
			if (n_leng > 4) {
				return 0;
			}
			tm->tm_year = date - 1900;
			return n_leng;

		case 'H':
			if (date > 23) {
				return 0;
			}
			tm->tm_hour = date;
			return n_leng;

		case 'I':
			if ((date > 12) || (date == 0)) {
				return 0;
			}
			tm->tm_hour = date;
			return n_leng;

		case 'M':
			if (date > 59) {
				return 0;
			}
			tm->tm_min = date;
			return n_leng;

		case 'S':
			if (date > 60) {
				return 0;
			}
			tm->tm_sec = date;
			return n_leng;

		case 'j':
			if ((date > 366) || (date == 0)) {
				return 0;
			}
			tm->tm_yday = date - 1;
			return n_leng;

		case 'd':
		case 'e':
			if ((date > 31) || (date == 0)) {
				return 0;
			}
			tm->tm_mday = date;
			return n_leng;

		case 'C':
			if (date > 99) {
				return 0;
			}
			tm->tm_year = date * 100 - 1900;
			return n_leng;

		case 'U':
		case 'W':
			if (date > 53) {
				return 0;
			}
			return n_leng;

		case 'n':
		case 't':
			while (isspace(s[count_equi]) != 0) {
				count_equi++;
			}
			return count_equi;

		case 'R':
		case 'T':
			res_equi = def_format("H", &s[0], tm);
			count_equi = res_equi;
			if (res_equi == 0) {
				return 0;
			}
			if (s[res_equi] != ':' ) {
				return 0;
			}
			res_equi = def_format("M", &s[count_equi + 1], tm);
			count_equi += res_equi + 1;
			if (res_equi == 0) {
				return 0;
			}
			if (format[0] == 'R') {
				return res_equi;
			}
			if (s[count_equi] != ':' ) {
				return 0;
			}
			res_equi = def_format("S", &s[count_equi + 1], tm);
			count_equi += res_equi + 1;
			if (res_equi == 0) {
				return 0;
			}
			return count_equi;

		case 'D':
			res_equi = def_format("m", &s[0], tm);
			count_equi = res_equi;
			if (res_equi == 0) {
				return 0;
			}
			if (s[res_equi] != '/' ) {
				return 0;
			}
			res_equi = def_format("d", &s[count_equi + 1], tm);
			count_equi += res_equi + 1;
			if (res_equi == 0) {
				return 0;
			}
			if (s[count_equi] != '/' ) {
				return 0;
			}
			res_equi = def_format("y", &s[count_equi + 1], tm);
			count_equi += res_equi + 1;
			if (res_equi == 0) {
				return 0;
			}
			return count_equi;

		default:
			return 0;
	}
}

/**
 * The strptime implementation doesn't support such format characters as %c, %p, %r, %U, %W, %x, %X.
 * Also it doesn't support all alternative locale-dependent versions of date and time representation
 * and all format characters of glibc.
 */

char *strptime(const char *s, const char *format, struct tm *tm) {

	int count_s = 0;
	int count_f = 0;
	int new_count;

	while (s[count_s] != '\0') {
		new_count = cmp_before_percent(&s[count_s], &format[count_f]);
		if ((new_count == 0) && (format[count_f] != '%')) {
			return NULL;
		}
		count_f += new_count;
		count_s += new_count;

		if ((format[count_f + 1] != 't') && (format[count_f + 1] != 'n')) {
			while (isspace(s[count_s]) != 0) {
				count_s++;
			}
		}
		new_count = def_format(&format[count_f + 1], &s[count_s], tm);
		if ((new_count == 0) && ((format[count_f + 1] != 't') && (format[count_f + 1] != 'n'))) {
			return NULL;
		}
		count_f += 2;
		count_s += new_count;
		if (format[count_f] == '\0') {
			return (char *) &s[count_s];
		}
	}
	if (format[count_f] == '\0') {
		return (char *) &s[count_s];
	}
	return NULL;
}

