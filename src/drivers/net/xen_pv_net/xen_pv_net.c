/**
 * @file
 * @brief Xen PV net driver
 *
 * @author
 * @date
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <embox/unit.h>
#include <xenstore.h>
#include <kernel/printk.h>

#define XS_MAX_KEY_LENGTH 		256
#define XS_MAX_VALUE_LENGTH		1024
#define XS_MAX_NAME_LENGTH		16
#define XS_MAX_DOMID_LENGTH		4

#define XS_LS_NOT_RECURSIVE		0
#define XS_LS_RECURSIVE			1

EMBOX_UNIT_INIT(xennet_init);

int xenstore_ls_val(char *ls_key, int flag) {
	char ls_value[XS_MAX_VALUE_LENGTH];
	char key[XS_MAX_KEY_LENGTH], value[XS_MAX_VALUE_LENGTH];
	int r;
	char *output;

	memset(&ls_value, 0, XS_MAX_VALUE_LENGTH);
	r = xenstore_ls(ls_key, ls_value, XS_MAX_VALUE_LENGTH);

	if (r <= 0)
		return r;

	output = ls_value;
	while(*output != 0) {
		int s;
		memset(&key, 0, XS_MAX_KEY_LENGTH);
		memset(&value, 0, XS_MAX_VALUE_LENGTH);
		sprintf(key, "%s/%s", ls_key, output);
		s = xenstore_read(key, value, XS_MAX_VALUE_LENGTH);

		if (s < 0) {
			printk("Can't read %s. Error %i\n", key, r);
			output += strlen(output) + 1;
			continue;
		}

		printk("%s = \"%s\"\n", key, value);

		if (flag == XS_LS_RECURSIVE)
			xenstore_ls_val(key, XS_LS_RECURSIVE);

		output += strlen(output) + 1;
	}
	return r;
}

static void xenstore_info() {
	char key[XS_MAX_KEY_LENGTH];
	char name[XS_MAX_NAME_LENGTH], domid[XS_MAX_DOMID_LENGTH];
	int r;

	printk("\n --- XenStore Info Begin ---\n");

	memset(&name, 0, XS_MAX_NAME_LENGTH);
	r = xenstore_read("name", name, XS_MAX_NAME_LENGTH);

	if (r < 0)
		printk("Can't retrieve name. Error %i\n", r);

	printk("\nname = %s\n", name);

	memset(&domid, 0, XS_MAX_DOMID_LENGTH);
	r = xenstore_read("domid", domid, XS_MAX_DOMID_LENGTH);
	
	if (r < 0)
		printk("Can't retrieve domid. Error %i\n", r);

	printk("\ndomid = %s\n", domid);

	printk("\nDomain Home Path contents:\n");
	memset(&key, 0, XS_MAX_KEY_LENGTH);
	sprintf(key, "/local/domain/%s", domid);
	r = xenstore_ls_val(key, XS_LS_RECURSIVE);
	
	if (r < 0)
		printk("Can't retrieve /local/domain/%s contents. Error %i\n", domid, r);

	printk("\n --- XenStore Info End ---\n");
}

static int xennet_init(void) {
	xenstore_info();

    return 0;
}
