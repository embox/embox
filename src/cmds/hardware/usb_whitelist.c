/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.01.2014
 */

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <drivers/usb/usb_whitelist_dev.h>
#include <embox/cmd.h>

EMBOX_CMD(usb_whitelist_main);

static void usb_wl_help(char *arg0) {

}

static void usb_wl_strfield(char *strfield, size_t strfieldn,
		const char *fmt,
		unsigned long val, unsigned long maxval) {

	if (val == maxval) {
		strcpy(strfield, "*");
	} else {
		snprintf(strfield, strfieldn, fmt, val);
	}
}

static int usb_wl_print(int fdwl) {
	struct usb_whitelist_rule *wl_rules;
	int rules_n;
	int i, ret;

	ret = ioctl(fdwl, USB_WHITELIST_IO_GETN, &rules_n);
	if (ret < 0 || rules_n <= 0) {
		return ret;
	}

	wl_rules = calloc(rules_n, sizeof(struct usb_whitelist_rule));
	if (!wl_rules) {
		return -ENOMEM;
	}

	if (0 > (ret = read(fdwl, wl_rules,
				rules_n * sizeof(struct usb_whitelist_rule)))) {
		free(wl_rules);
		return ret;
	}

	printf("%4s\t%4s\t%4s\t%4s\n", "ID", "PID", "VID", "S/N");
	for (i = 0; i < rules_n; i++) {
		struct usb_whitelist_rule *wl_rule = wl_rules + i;
		char strpid[5];
		char strsn[5];

		usb_wl_strfield(strpid, 5, "%x", wl_rule->pid, USB_WHITELIST_PID_ANY);
		usb_wl_strfield(strsn, 5, "%d", wl_rule->sn, USB_WHITELIST_SN_ANY);

		printf("%4u\t%4x\t%4s\t%4s\n", wl_rule->id, wl_rule->vid, strpid, strsn);
	}

	free(wl_rules);
	return 0;
}

static int usb_wl_rule_parse(char *strrule[], int strn,
		struct usb_whitelist_rule *wl_rule) {
	unsigned long vals[3];
	int i;

	if (strn <= 0 || strn > 3) {
		return -EINVAL;
	}

	memset(vals, 0xff, sizeof(vals));

	for (i = 0; i < strn; i++) {
		char *endf;
		vals[i] = strtol(strrule[i], &endf, 0);
		if (*endf) {
			return -EINVAL;
		}
	}

	wl_rule->vid = vals[0];
	wl_rule->pid = vals[1];
	wl_rule->sn  = vals[2];

	return 0;
}

#define USB_WL_DEV_PATH_LEN 64
static int usb_whitelist_main(int argc, char *argv[]) {
	struct usb_whitelist_rule wl_rule;
	char usb_wl_dev_path[USB_WL_DEV_PATH_LEN];
	int rule_id;
	int fdwl;
	int ret;

	if (argc < 2 || *argv[1] != '-') {
		usb_wl_help(argv[0]);
		return 0;
	}

	strncpy(usb_wl_dev_path, "/dev/", USB_WL_DEV_PATH_LEN);
	strncat(usb_wl_dev_path, USB_WHITELIST_DEV_NAME, USB_WL_DEV_PATH_LEN);

	fdwl = open(usb_wl_dev_path, O_RDONLY);
	if (0 > fdwl) {
		return -errno;
	}

	switch (argv[1][1]) {
	case 'P':
		ret = usb_wl_print(fdwl);
		break;
	case 'A':
		if ((ret = usb_wl_rule_parse(argv + 2, argc - 2, &wl_rule))) {
			usb_wl_help(argv[0]);
			break;
		}

		ret = ioctl(fdwl, USB_WHITELIST_IO_ADD, &wl_rule);
		break;
	case 'D':
		if (argc != 3) {
			ret = -EINVAL;
			break;
		}

		rule_id = atoi(argv[2]);
		ret = ioctl(fdwl, USB_WHITELIST_IO_DEL, &rule_id);
		break;
	case 'F':
		ret = ioctl(fdwl, USB_WHITELIST_IO_FLUSH);
		break;
	}

	close(fdwl);

	return ret;
}
