/**
 * @file
 *
 * @date Oct 21, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fs/idesc.h>

int idesc_init(struct idesc *idesc, struct permissions *attr) {
	return -ENOSUPP;
}


int idesc_read(struct idesc *idesc, char *buff, int size) {
	return -ENOSUPP;
}

int idesc_write(struct idesc *idesc,char *buff, int size) {
	return -ENOSUPP;
}

int idesc_ctrl(struct idesc *idesc, int cmd, int type, void *buff, int size) {
	return -ENOSUPP;
}

