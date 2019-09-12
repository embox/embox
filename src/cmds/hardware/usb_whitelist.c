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

#include <lib/crypt/crc32.h>

#include <drivers/usb/usb_whitelist_dev.h>

#define USB_WL_DUMP_HDR_MAGIC_LEN 4
static const char usb_wl_dump_hdr_magic[] = "UWLD";

struct usb_wl_dump_hdr {
	char df_magic[4];
	unsigned long df_crc32;
} __attribute__((packed));

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

static int usb_wl_load(int fdwl, struct usb_whitelist_rule **wl_rules_ptr,
		int *rules_n_ptr) {
	int ret;
	int rules_n;
	struct usb_whitelist_rule *wl_rules;

	ret = ioctl(fdwl, USB_WHITELIST_IO_GETN, &rules_n);
	if (ret < 0)
		return ret;

	if (rules_n == 0) {
		*rules_n_ptr = 0;
		return 0;
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

	*wl_rules_ptr = wl_rules;
	*rules_n_ptr = rules_n;

	return 0;
}

static int usb_wl_print(int fdwl) {
	struct usb_whitelist_rule *wl_rules;
	int rules_n;
	int i, ret;

	if ((ret = usb_wl_load(fdwl, &wl_rules, &rules_n))) {
		return ret;
	}

	printf("%4s\t%4s\t%4s\t%4s\n", "ID", "PID", "VID", "S/N");
	for (i = 0; i < rules_n; i++) {
		struct usb_whitelist_rule *wl_rule = wl_rules + i;
		char strpid[5];

		usb_wl_strfield(strpid, 5, "%x", wl_rule->pid,
				USB_WHITELIST_PID_ANY);

		printf("%4u\t%4x\t%4s\t%4s\n", wl_rule->id, wl_rule->vid,
				strpid, wl_rule->sn);
	}

	if (rules_n)
		free(wl_rules);
	return 0;
}

static void usb_wl_dump_hdr_init(const struct usb_whitelist_rule *wl_rules,
		int wl_rules_n, struct usb_wl_dump_hdr *df_header) {

	memcpy(df_header->df_magic, usb_wl_dump_hdr_magic,
			USB_WL_DUMP_HDR_MAGIC_LEN);
	df_header->df_crc32 = count_crc32((void *) wl_rules,
			(void *) (wl_rules + wl_rules_n));
}

static int usb_wl_dump_hdr_check(const struct usb_whitelist_rule *wl_rules,
		int wl_rules_n, const struct usb_wl_dump_hdr *df_header) {

	return 0 == memcmp(df_header->df_magic, usb_wl_dump_hdr_magic,
			USB_WL_DUMP_HDR_MAGIC_LEN)
		&& df_header->df_crc32 == count_crc32((void *) wl_rules,
				(void *) (wl_rules + wl_rules_n));
}

static int usb_wl_dump_save(int fdwl, const char *filename) {
	struct usb_wl_dump_hdr df_header;
	struct usb_whitelist_rule *wl_rules;
	int rules_n;
	int ret;
	int fd;

	if ((ret = usb_wl_load(fdwl, &wl_rules, &rules_n))) {
		return ret;
	}

	fd = creat(filename, S_IRALL | S_IWUSR);
	if (fd < 0) {
		ret = -errno;
		goto out;
	}

	usb_wl_dump_hdr_init(wl_rules, rules_n, &df_header);

	ret = write(fd, &df_header, sizeof(df_header));
	if (0 > ret) {
		ret = -errno;
		goto out;
	}

	ret = write(fd, wl_rules, sizeof(struct usb_whitelist_rule) * rules_n);
	if (0 > ret) {
		ret = -errno;
		goto out;
	}

	ret = 0;
out:
	if (rules_n)
		free(wl_rules);
	return ret;
}

static int usb_wl_dump_load(int fdwl, const char *filename) {
	struct usb_wl_dump_hdr df_header;
	struct usb_whitelist_rule *wl_rules;
	int rules_n;
	size_t filelen;
	int i, ret;
	int fd;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		return -errno;
	}

	filelen = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	if ((filelen - sizeof(struct usb_wl_dump_hdr)) %
			sizeof(struct usb_whitelist_rule)) {
		ret = -EINVAL;
		goto out_file;
	}

	rules_n = (filelen - sizeof(struct usb_wl_dump_hdr)) /
			sizeof(struct usb_whitelist_rule);

	if (0 > read(fd, &df_header, sizeof(df_header))) {
		ret = -errno;
		goto out_file;
	}

	wl_rules = calloc(rules_n, sizeof(struct usb_whitelist_rule));
	if (!wl_rules) {
		ret = -ENOMEM;
		goto out_file_mem;
	}

	if (0 > read(fd, wl_rules, sizeof(struct usb_whitelist_rule) * rules_n)) {
		ret = -errno;
		goto out_file_mem;
	}

	if (!usb_wl_dump_hdr_check(wl_rules, rules_n, &df_header)) {
		ret = -EINVAL;
		goto out_file_mem;
	}

	ioctl(fdwl, USB_WHITELIST_IO_FLUSH);
	for (i = 0; i < rules_n; i++) {
		ioctl(fdwl, USB_WHITELIST_IO_ADD, wl_rules + i);
	}

	ret = 0;
out_file_mem:
	free(wl_rules);
out_file:
	close(fd);
	return ret;
}
static int usb_wl_rule_parse(char *strrule[], int strn,
		struct usb_whitelist_rule *wl_rule) {
	char *endf;

	if (strn <= 0 || strn > 3) {
		return -EINVAL;
	}

	wl_rule->vid = wl_rule->pid = ~0;
	strcpy(wl_rule->sn, USB_WHITELIST_SN_ANY);

	wl_rule->vid = strtol(strrule[0], &endf, 0);
	if (*endf) {
		return -EINVAL;
	}

	if (!--strn) {
		return 0;
	}
	wl_rule->pid = strtol(strrule[1], &endf, 0);
	if (*endf) {
		return -EINVAL;
	}

	if (!--strn) {
		return 0;
	}
	strncpy(wl_rule->sn, strrule[2], USB_WHITELIST_SN_LEN - 1);
	wl_rule->sn[USB_WHITELIST_SN_LEN - 1] = '\0';

	return 0;
}

#define USB_WL_DEV_PATH_LEN 64
#define USB_WL_DEVFS_PREFIX "/dev/"
int main(int argc, char *argv[]) {
	struct usb_whitelist_rule wl_rule;
	char usb_wl_dev_path[USB_WL_DEV_PATH_LEN];
	int rule_id;
	int fdwl;
	int ret;

	if (argc < 2 || *argv[1] != '-') {
		usb_wl_help(argv[0]);
		return 0;
	}

	strncpy(usb_wl_dev_path,
			USB_WL_DEVFS_PREFIX,
			sizeof(usb_wl_dev_path) - 1);
	usb_wl_dev_path[sizeof(usb_wl_dev_path) - 1] = '\0';

	strncat(usb_wl_dev_path,
			USB_WHITELIST_DEV_NAME,
			sizeof(usb_wl_dev_path) - 1 - strlen(USB_WL_DEVFS_PREFIX));

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
	case 'S':
		ret = usb_wl_dump_save(fdwl, argv[2]);
		break;
	case 'L':
		ret = usb_wl_dump_load(fdwl, argv[2]);
		break;

	}

	close(fdwl);

	return ret;
}
