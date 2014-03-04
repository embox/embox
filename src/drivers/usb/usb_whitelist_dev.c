/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.01.2014
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fs/file_operation.h>
#include <embox/device.h>
#include <sys/ioctl.h>
#include <util/math.h>

#include <drivers/usb/usb_whitelist_dev.h>
#include <drivers/usb/usb.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(usb_whitelist_dev_init);

static const char *builtin_whitelist = OPTION_STRING_GET(whitelist);

#define USB_WHITELIST_MAX_RULES 16

static struct usb_whitelist_conf {
	unsigned int next_id;
	int rules_n;
	struct usb_whitelist_rule rules[USB_WHITELIST_MAX_RULES];
} whitelist_conf;

static int usb_whitelist_conf_init(struct usb_whitelist_conf *wl_conf) {
	wl_conf->rules_n = 0;
	wl_conf->next_id = 1;
	return 0;
}

static int usb_whitelist_conf_add(struct usb_whitelist_conf *wl_conf,
		struct usb_whitelist_rule *wl_rule) {
	struct usb_whitelist_rule *wl_rule_place;

	if (wl_conf->rules_n == USB_WHITELIST_MAX_RULES) {
		return -ENOMEM;
	}

	wl_rule_place = &wl_conf->rules[wl_conf->rules_n];
	memcpy(wl_rule_place, wl_rule, sizeof(struct usb_whitelist_rule));

	wl_rule_place->id = wl_conf->next_id++;
	wl_conf->rules_n++;
	return 0;
}

static int usb_whitelist_conf_del(struct usb_whitelist_conf *wl_conf,
		int id) {
	int i;

	for (i = 0; i < wl_conf->rules_n; i++) {
		struct usb_whitelist_rule *wl_rule = &wl_conf->rules[i];

		if (wl_rule->id == id) {

			--wl_conf->rules_n;
			memmove(&wl_conf->rules[i], &wl_conf->rules[i + 1],
			       (wl_conf->rules_n - i) * sizeof(struct usb_whitelist_rule));
			return 0;
		}
	}

	return -ENOENT;
}

static int usb_whitelist_conf_isin(struct usb_whitelist_conf *wl_conf,
		struct usb_desc_device *desc) {
	int i;

	for (i = 0; i < wl_conf->rules_n; i++) {
		struct usb_whitelist_rule *wl_rule = &wl_conf->rules[i];

		if (desc->id_vendor == wl_rule->vid
				&& (wl_rule->pid == USB_WHITELIST_PID_ANY
					|| wl_rule->pid == desc->id_product)
				&& (wl_rule->sn == USB_WHITELIST_SN_ANY
					|| wl_rule->sn == desc->i_serial_number)) {
			return 1;
		}
	}

	return 0;
}

int usb_whitelist_check(struct usb_desc_device *desc) {
	struct usb_whitelist_conf *wl_conf = &whitelist_conf;

	if (wl_conf->rules_n) {
		char in_list = usb_whitelist_conf_isin(wl_conf, desc);

		return in_list ? 0 : -EACCES;
	}

	return 0;
}

static int usb_whitelist_open(struct node *node, struct file_desc *file_desc, int flags) {
	return 0;
}

static int usb_whitelist_close(struct file_desc *desc) {
	return 0;
}

static size_t usb_whitelist_read(struct file_desc *desc, void *buf, size_t size) {
	struct usb_whitelist_conf *wl_conf = &whitelist_conf;
	int req_rules = min(size / sizeof(struct usb_whitelist_rule), wl_conf->rules_n);
	size_t req_size =  req_rules * sizeof(struct usb_whitelist_rule);

	memcpy(buf, whitelist_conf.rules, req_size);

	return req_size;
}

static int usb_whitelist_ioctl(struct file_desc *desc, int request, ...) {
	struct usb_whitelist_conf *wl_conf = &whitelist_conf;
	struct usb_whitelist_rule *wl_rule = NULL;
	int rule_id;
	va_list va;
	int ret;

	va_start(va, request);

	switch (request) {
	case USB_WHITELIST_IO_ADD:
		wl_rule = va_arg(va, struct usb_whitelist_rule *);
		ret = usb_whitelist_conf_add(wl_conf, wl_rule);
		break;
	case USB_WHITELIST_IO_DEL:
		rule_id = * va_arg(va, int *);
		ret = usb_whitelist_conf_del(wl_conf, rule_id);
		break;
	case USB_WHITELIST_IO_FLUSH:
		ret = usb_whitelist_conf_init(wl_conf);
		break;
	case USB_WHITELIST_IO_GETN:
		* va_arg(va, int *) = wl_conf->rules_n;
		ret = 0;
		break;
	default:
		ret = -ENOSYS;
		break;
	}

	va_end(va);

	return ret;
}

static const struct kfile_operations usb_whitelist_ops = {
	.open  = usb_whitelist_open,
	.ioctl = usb_whitelist_ioctl,
	.read  = usb_whitelist_read,
	.close = usb_whitelist_close,
};

static void usb_whitelist_parse_builtin(struct usb_whitelist_conf *wl_conf,
		const char *builtin_whitelist) {
	const char *p;

	usb_whitelist_conf_init(wl_conf);

	p = builtin_whitelist;
	while (*p) {
		struct usb_whitelist_rule wl_rule;
		unsigned long vals[3] = {0x0, USB_WHITELIST_PID_ANY, USB_WHITELIST_SN_ANY};
		int vals_cnt = 0;

		while (*p) {
			char *pp;

			vals[vals_cnt++] = strtol(p, &pp, 0);
			if (p == pp) {
				goto next_dev;
			}

			p = pp;
			if (*p != ':')
				break;
			if (vals_cnt == 3)
				break;
			++p;
		}

		wl_rule.vid = vals[0];
		wl_rule.pid = vals[1];
		wl_rule.sn = vals[2];

		usb_whitelist_conf_add(wl_conf, &wl_rule);

next_dev:
		if (*p != ',')
			break;
		++p;
	}
}

static int usb_whitelist_dev_init(void) {
	struct usb_whitelist_conf *wl_conf = &whitelist_conf;

	usb_whitelist_parse_builtin(wl_conf, builtin_whitelist);

	return char_dev_register(USB_WHITELIST_DEV_NAME, &usb_whitelist_ops);
}
