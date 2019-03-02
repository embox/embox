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

#define MAX_KEY_LENGTH 		64
#define MAX_VALUE_LENGTH 	64

EMBOX_UNIT_INIT(xennet_init);

static int xennet_init(void) {
	char key[MAX_KEY_LENGTH], value[MAX_VALUE_LENGTH],
		 domid[8], values[128];
	int r;

	memset(&key, 0, MAX_KEY_LENGTH);
	memset(&value, 0, MAX_VALUE_LENGTH);
	strcpy(key, "vm");
	xenstore_read(key, value, MAX_VALUE_LENGTH);
	printk("\n\n"
		   "--- KEY:%s ---\n"
		   "--- VALUE:%s ---\n\n",
		   key, value);

	memset(&key, 0, MAX_KEY_LENGTH);
	memset(&value, 0, MAX_VALUE_LENGTH);
	strcpy(key, "name");
	xenstore_read(key, value, MAX_VALUE_LENGTH);
	printk("\n\n"
		   "--- KEY:%s ---\n"
		   "--- VALUE:%s ---\n\n",
		   key, value);

	memset(&key, 0, MAX_KEY_LENGTH);
	memset(&value, 0, MAX_VALUE_LENGTH);
	strcpy(key, "domid");
	xenstore_read(key, value, MAX_VALUE_LENGTH);
	printk("\n\n"
		   "--- KEY:%s ---\n"
		   "--- VALUE:%s ---\n\n",
		   key, value);
	strcpy(domid, value);

	memset(&key, 0, MAX_KEY_LENGTH);
	memset(&value, 0, MAX_VALUE_LENGTH);
	strcpy(key, "memory/static-max");
	xenstore_read(key, value, MAX_VALUE_LENGTH);
	printk("\n\n"
		   "--- KEY:%s ---\n"
		   "--- VALUE:%s ---\n\n",
		   key, value);

	memset(&key, 0, MAX_KEY_LENGTH);
	memset(&values, 0, 128);
	sprintf(key, "console");
	r = xenstore_ls(key, values, MAX_VALUE_LENGTH);
	if (r < 0) {
		printk("\n\nERROR\n\n");
		return 0;
	}
	printk("\n\n"
		   "--- KEY:%s ---\n"
		   "--- VALUE:\n%s ---\n\n",
		   key, values);


    return 0;
}