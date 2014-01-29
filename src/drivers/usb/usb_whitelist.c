/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.01.2014
 */

#include <errno.h>
#include <stdlib.h>
#include <framework/mod/options.h>
#include <drivers/usb/usb.h>

#define BASE 16
static const char *whitelist = OPTION_STRING_GET(whitelist);

static int usb_inlist(const char *list, unsigned short vid, unsigned short pid) {
	unsigned short lvid, lpid;
	const char *p;
        char *pp;

	p = list;
	do {
		lvid = strtol(p, &pp, BASE);
		if (p == pp || *pp != ':')
			break;
		p = pp + 1;

		lpid = strtol(p, &pp, BASE);
		if (p == pp)
			break;
		p = pp + 1;

		if (vid == lvid && pid == lpid) {
			return 1;
		}
	} while (*pp == ',');

	return 0;
}

int usb_whitelist_check(struct usb_desc_device *desc) {

	if (*whitelist) {
		char in_list = usb_inlist(whitelist, desc->id_vendor, desc->id_product);

		return in_list ? 0 : -EACCES;
	}

	return 0;
}
