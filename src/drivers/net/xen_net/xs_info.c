/**
 * @file 
 * @brief Shows information from XenStore
 *
 * @date 
 * @author 
 */

#include <stdio.h>
#include <string.h>
#include <kernel/printk.h>
#include <xenstore.h>

#include "xs_info.h"

static int xenstore_ls_val(char *ls_key, int flag) {
	char ls_value[XS_VALUE_LEN];
	char key[XS_KEY_LEN], value[XS_VALUE_LEN];
	int r;
	char *output;

	memset(&ls_value, 0, XS_VALUE_LEN);
	r = xenstore_ls(ls_key, ls_value, XS_VALUE_LEN);

	if (r <= 0)
		return r;

	output = ls_value;
	while(*output != 0) {
		int s;
		memset(&key, 0, XS_KEY_LEN);
		memset(&value, 0, XS_VALUE_LEN);
		sprintf(key, "%s/%s", ls_key, output);
		s = xenstore_read(key, value, XS_VALUE_LEN);

		if (s < 0) {
			printk("Can't read %s. Error %i\n", key, r);
			output += strlen(output) + 1;
			continue;
		}

		printk("%s = \"%s\"\n", key, value);

		if (flag == XS_LS_REC)
			xenstore_ls_val(key, XS_LS_REC);

		output += strlen(output) + 1;
	}
	return r;
}

void xenstore_info() {
	char key[XS_KEY_LEN];
	char name[XS_NAME_LEN], domid[XS_DOMID_LEN];
	int r;

	printk("\n --- XenStore Info Begin ---\n");

	memset(&name, 0, XS_NAME_LEN);
	r = xenstore_read("name", name, XS_NAME_LEN);

	if (r < 0)
		printk("Can't retrieve name. Error %i\n", r);

	printk("\nname = %s\n", name);

	memset(&domid, 0, XS_DOMID_LEN);
	r = xenstore_read("domid", domid, XS_DOMID_LEN);

	if (r < 0)
		printk("Can't retrieve domid. Error %i\n", r);

	printk("\ndomid = %s\n", domid);

	printk("\nDomain Home Path contents:\n");
	memset(&key, 0, XS_KEY_LEN);
	sprintf(key, "/local/domain/%s", domid);
	r = xenstore_ls_val(key, XS_LS_REC);

	if (r < 0) {
		printk("Can't retrieve /local/domain/%s contents. Error %i\n",
				domid, r);
	}

	printk("\n --- XenStore Info End ---\n");
}