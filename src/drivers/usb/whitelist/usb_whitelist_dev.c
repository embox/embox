/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.01.2014
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/usb/usb.h>
#include <drivers/usb/usb_whitelist_dev.h>
#include <embox/unit.h>
#include <mem/misc/pool.h>
#include <util/macro.h>
#include <util/math.h>

EMBOX_UNIT_INIT(usb_whitelist_dev_init);

struct usb_whitelist_temp_string {
	short lang_array[2];
	char str_sn[2 * USB_WHITELIST_SN_LEN];
};

POOL_DEF(usb_whitelist_temp_strings, struct usb_whitelist_temp_string, 16);

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

static int usb_whitelist_conf_del(struct usb_whitelist_conf *wl_conf, int id) {
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
    struct usb_desc_device *desc, const char *desc_sn) {
	int i;

	for (i = 0; i < wl_conf->rules_n; i++) {
		struct usb_whitelist_rule *wl_rule = &wl_conf->rules[i];

		if (desc->id_vendor == wl_rule->vid
		    && (wl_rule->pid == USB_WHITELIST_PID_ANY
		        || wl_rule->pid == desc->id_product)
		    && (!strcmp(wl_rule->sn, USB_WHITELIST_SN_ANY)
		        || !strcmp(wl_rule->sn, desc_sn))) {
			return 1;
		}
	}

	return 0;
}

static void usb_wl_dev_check(struct usb_dev *dev, const char *sn) {
	char ok;

	ok = whitelist_conf.rules_n == 0
	         ? 1
	         : usb_whitelist_conf_isin(&whitelist_conf, &dev->dev_desc, sn);

	if (ok) {
		usb_whitelist_accepts(dev);
	}
	else {
		usb_whitelist_rejects(dev);
	}
}

static void usb_wl_sn_got(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_whitelist_temp_string *tstrs = arg;
	int i, j, len;

	len = tstrs->str_sn[0];
	for (i = 0, j = 2; j < len; i++, j += 2) {
		tstrs->str_sn[i] = tstrs->str_sn[j];
	}
	tstrs->str_sn[i] = '\0';

	usb_wl_dev_check(dev, tstrs->str_sn);

	pool_free(&usb_whitelist_temp_strings, tstrs);
}

static void usb_wl_sn_len_got(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_whitelist_temp_string *tstrs = arg;
	short lang_id = tstrs->lang_array[1];

	usb_endp_control(&req->endp->dev->endp0, usb_wl_sn_got, tstrs,
	    USB_DEV_REQ_TYPE_RD | USB_DEV_REQ_TYPE_STD | USB_DEV_REQ_TYPE_DEV,
	    USB_DEV_REQ_GET_DESC,
	    USB_DESC_TYPE_STRING << 8 | dev->dev_desc.i_serial_number, lang_id,
	    tstrs->str_sn[0], &tstrs->str_sn);
}

static void usb_wl_lang_array_got(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_whitelist_temp_string *tstrs = arg;
	short lang_id = tstrs->lang_array[1];

	usb_endp_control(&req->endp->dev->endp0, usb_wl_sn_len_got, tstrs,
	    USB_DEV_REQ_TYPE_RD | USB_DEV_REQ_TYPE_STD | USB_DEV_REQ_TYPE_DEV,
	    USB_DEV_REQ_GET_DESC,
	    USB_DESC_TYPE_STRING << 8 | dev->dev_desc.i_serial_number, lang_id, 2,
	    &tstrs->str_sn);
}

int usb_whitelist_check(struct usb_dev *dev) {
	struct usb_whitelist_temp_string *tstrs;

	if (!dev->dev_desc.i_serial_number) {
		usb_wl_dev_check(dev, USB_WHITELIST_SN_ANY);
		return 0;
	}

	tstrs = pool_alloc(&usb_whitelist_temp_strings);
	if (!tstrs) {
		return -ENOMEM;
	}

	memset(tstrs, 0, sizeof(*tstrs));

	usb_endp_control(&dev->endp0, usb_wl_lang_array_got, tstrs,
	    USB_DEV_REQ_TYPE_RD | USB_DEV_REQ_TYPE_STD | USB_DEV_REQ_TYPE_DEV,
	    USB_DEV_REQ_GET_DESC, USB_DESC_TYPE_STRING << 8, 0,
	    sizeof(tstrs->lang_array), &tstrs->lang_array);

	return -EBUSY;
}

static ssize_t usb_whitelist_read(struct char_dev *cdev, void *buf,
    size_t nbyte) {
	struct usb_whitelist_conf *wl_conf;
	int req_rules;
	ssize_t req_size;

	wl_conf = &whitelist_conf;
	req_rules = min(nbyte / sizeof(struct usb_whitelist_rule),
	    wl_conf->rules_n);

	req_size = req_rules * sizeof(struct usb_whitelist_rule);

	memcpy(buf, whitelist_conf.rules, req_size);

	return req_size;
}

static int usb_whitelist_ioctl(struct char_dev *cdev, int request, void *data) {
	struct usb_whitelist_conf *wl_conf = &whitelist_conf;
	struct usb_whitelist_rule *wl_rule = NULL;
	int rule_id;
	int ret;

	switch (request) {
	case USB_WHITELIST_IO_ADD:
		wl_rule = data;
		ret = usb_whitelist_conf_add(wl_conf, wl_rule);
		break;
	case USB_WHITELIST_IO_DEL:
		rule_id = *((int *)data);
		ret = usb_whitelist_conf_del(wl_conf, rule_id);
		break;
	case USB_WHITELIST_IO_FLUSH:
		ret = usb_whitelist_conf_init(wl_conf);
		break;
	case USB_WHITELIST_IO_GETN:
		*((int *)data) = wl_conf->rules_n;
		ret = 0;
		break;
	default:
		ret = -ENOSYS;
		break;
	}

	return ret;
}

static void usb_whitelist_parse_builtin(struct usb_whitelist_conf *wl_conf,
    const char *builtin_whitelist) {
	char scratch[64];
	const char *p;

	usb_whitelist_conf_init(wl_conf);

	p = builtin_whitelist;
	while (p && *p) {
		const char *comma = strchr(p, ',');
		struct usb_whitelist_rule wl_rule;
		char *pp, *str, *saveptr, *tok[3];
		int tok_i, actlen;

		actlen = comma ? min(comma - p, sizeof(scratch) - 1)
		               : sizeof(scratch) - 1;
		strncpy(scratch, p, actlen);
		scratch[actlen] = '\0';

		if (comma) {
			p = comma + 1;
		}
		else {
			p = NULL;
		}

		for (str = scratch, tok_i = 0; tok_i < 3; str = NULL, tok_i++) {
			tok[tok_i] = strtok_r(str, ":", &saveptr);
		}

		if (tok[0]) {
			wl_rule.vid = strtol(tok[0], &pp, 0);
			if (pp == tok[0]) {
				continue;
			}
		}
		else {
			continue;
		}

		if (tok[1]) {
			wl_rule.pid = strtol(tok[1], &pp, 0);
			if (pp == tok[1]) {
				continue;
			}
		}
		else {
			wl_rule.pid = USB_WHITELIST_PID_ANY;
		}

		if (tok[2]) {
			strncpy(wl_rule.sn, tok[2], USB_WHITELIST_SN_LEN - 1);
			wl_rule.sn[USB_WHITELIST_SN_LEN - 1] = '\0';
		}
		else {
			strcpy(wl_rule.sn, USB_WHITELIST_SN_ANY);
		}

		usb_whitelist_conf_add(wl_conf, &wl_rule);
	}
}

static int usb_whitelist_dev_init(void) {
	struct usb_whitelist_conf *wl_conf = &whitelist_conf;

	usb_whitelist_parse_builtin(wl_conf, builtin_whitelist);

	return 0;
}

static const struct char_dev_ops usb_whitelist_cdev_ops = {
    .ioctl = usb_whitelist_ioctl,
    .read = usb_whitelist_read,
};

static struct char_dev usb_whitelist_cdev = CHAR_DEV_INIT(zero_cdev, "zero",
    &zero_cdev_ops);

CHAR_DEV_REGISTER(&usb_whitelist_cdev);
