/**
 * @file
 *
 * @date Dec 18, 2013
 * @author: Anton Bondarev
 */
#include <assert.h>

#include <err.h>

#include <mem/misc/pool.h>
#include <fs/idesc.h>

#include <fs/idesc_serial.h>

#include <framework/mod/options.h>

#define MAX_SERIALS \
	OPTION_GET(NUMBER, serial_quantity);

POOL_DEF(pool_serials, struct idesc_serial, MAX_SERIALS);

struct idesc *idesc_serial_create(struct node *node, const char *name) {
	struct idesc_serial isesc;

	assert(node && name);


	if (node) {

	}
	return NULL;
}
