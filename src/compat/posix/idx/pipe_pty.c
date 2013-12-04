/**
 * @file
 *
 * @brief
 *
 * @date 17.11.2013
 * @author Anton Kozlov
 */

#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <poll.h>

#include <util/ring_buff.h>
#include <util/ring.h>
#include <util/member.h>

#include <drivers/pty.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <fs/idesc.h>
#include <fs/flags.h>

#include <kernel/task/idesc_table.h>

struct ppty;

struct idesc_ppty {
	struct idesc idesc;
	struct ppty *ppty;
};

struct ppty {
	struct pty pty;
	struct idesc *master, *slave;
};

static int ppty_close(struct idesc *idesc);
static int ppty_ioctl(struct idesc *idesc, int request, void *data);
static int ppty_slave_write(struct idesc *desc, const void *buf, size_t nbyte);
static int ppty_slave_read(struct idesc *idesc, void *buf, size_t nbyte);
static int ppty_master_write(struct idesc *desc, const void *buf, size_t nbyte);
static int ppty_master_read(struct idesc *idesc, void *buf, size_t nbyte);
static int ppty_fstat(struct idesc *data, void *buff);
static int ppty_master_status(struct idesc *idesc, int mask);
static int ppty_slave_status(struct idesc *idesc, int mask);

static const struct idesc_ops ppty_master_ops = {
		.write = ppty_master_write,
		.read  = ppty_master_read,
		.close = ppty_close,
		/*.ioctl = ppty_ioctl,*/
		/*.fstat = ppty_fstat,*/
		.status = ppty_master_status,
};

static const struct idesc_ops ppty_slave_ops = {
		.write  = ppty_slave_write,
		.read   = ppty_slave_read,
		.close  = ppty_close,
		.ioctl  = ppty_ioctl,
		.fstat  = ppty_fstat,
		.status = ppty_slave_status,
};

static struct ppty *ppty_create(void) {
	struct ppty *ppty;

	ppty = malloc(sizeof(struct ppty));

	if (ppty) {
		pty_init(&ppty->pty);
	}

	return ppty;
}

static void ppty_delete(struct ppty *ppty) {

	free(ppty);
}

static struct idesc_ppty *idesc_ppty_create(struct ppty *ppty, const struct idesc_ops *ops,
		struct idesc **idesc) {
	struct idesc_ppty *ippty;

	ippty = malloc(sizeof(struct idesc_ppty));

	if (ippty) {
		idesc_init(&ippty->idesc, ops, FS_MAY_READ | FS_MAY_WRITE);
	}

	ippty->ppty = ppty;

	*idesc = &ippty->idesc;

	return ippty;
}

static void idesc_ppty_delete(struct idesc_ppty *ppty, struct idesc **idesc) {

	*idesc = NULL;

	free(ppty);
}

static int ppty_fixup_error(struct idesc *idesc, int code) {
	struct ppty *ppty;
	struct idesc *idesc_other;

	/* Negative => error, positive => some data read */
	if (code != 0) {
		return code;
	}

	ppty = ((struct idesc_ppty *) idesc)->ppty;

	if (idesc == ppty->master) {
		idesc_other = ppty->slave;
	} else {
		assert(idesc == ppty->slave);
		idesc_other = ppty->master;
	}

	if (idesc_other == NULL) {
		return 0;
	}

	assert(idesc->idesc_flags & O_NONBLOCK);
	return -EAGAIN;
}

static int ppty_close(struct idesc *idesc) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) idesc;
	struct idesc **ippty_pm, **ippty_ps;
	struct ppty *ppty;

	ippty_pm = &ippty->ppty->master;
	ippty_ps = &ippty->ppty->slave;

	ppty = ippty->ppty;

	sched_lock();
	{
		if (idesc == *ippty_pm) {
			idesc_ppty_delete((struct idesc_ppty *) idesc, ippty_pm);
			/* Wake up writing end if it is sleeping. */
			/*event_notify(&pipe->write_wait);*/
		} else if (idesc == *ippty_ps) {
			idesc_ppty_delete((struct idesc_ppty *) idesc, ippty_ps);
			/* Wake up reading end if it is sleeping. */
			/*event_notify(&pipe->read_wait);*/
		}

		/* Free memory if both of ends are closed. */
		if (NULL == *ippty_pm && NULL == *ippty_ps) {
			ppty_delete(ppty);
		}

	}
	sched_unlock();

	return 0;
}

static int ppty_master_write(struct idesc *desc, const void *buf, size_t nbyte) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) desc;
	int res;

	/* XXX */
	pty_to_tty(&ippty->ppty->pty)->file_flags = desc->idesc_flags;

	res = pty_write(&ippty->ppty->pty, buf, nbyte);
	return ppty_fixup_error(desc, res);
}

static int ppty_master_read(struct idesc *desc, void *buf, size_t nbyte) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) desc;
	int res;

	/* XXX */
	pty_to_tty(&ippty->ppty->pty)->file_flags = desc->idesc_flags;

	res = pty_read(&ippty->ppty->pty, buf, nbyte);
	return ppty_fixup_error(desc, res);
}

static int ppty_slave_write(struct idesc *desc, const void *buf, size_t nbyte) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) desc;
	int res;

	/* XXX */
	pty_to_tty(&ippty->ppty->pty)->file_flags = desc->idesc_flags;

	res = tty_write(pty_to_tty(&ippty->ppty->pty), buf, nbyte);
	return ppty_fixup_error(desc, res);
}

static int ppty_slave_read(struct idesc *desc, void *buf, size_t nbyte) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) desc;
	int res;

	/* XXX */
	pty_to_tty(&ippty->ppty->pty)->file_flags = desc->idesc_flags;

	res = tty_read(pty_to_tty(&ippty->ppty->pty), buf, nbyte);
	return ppty_fixup_error(desc, res);
}

static int ppty_fstat(struct idesc *data, void *buff) {
	struct stat *st = buff;

	st->st_mode = S_IFCHR;

	return 0;

}

static int ppty_ioctl(struct idesc *idesc, int request, void *data) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) idesc;

	return tty_ioctl(pty_to_tty(&ippty->ppty->pty), request, data);
}

//TODO check it
static int ppty_master_status(struct idesc *idesc, int mask) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) idesc;
	struct pty *pty = &ippty->ppty->pty;
	int res;

	switch (mask) {
	case POLLIN:
		res = ring_can_read(&pty_to_tty(pty)->o_ring, TTY_IO_BUFF_SZ, 1);
	case POLLOUT:
		res = ring_can_write(&pty_to_tty(pty)->rx_ring, TTY_RX_BUFF_SZ, 1);
		break;
	default:
	case POLLERR:
		res = 0;
		break;
	}

	return res;
}

static int ppty_slave_status(struct idesc *idesc, int mask) {
	struct idesc_ppty *ippty = (struct idesc_ppty *) idesc;
	struct pty *pty = &ippty->ppty->pty;
	int res;

	switch (mask) {
	case POLLIN:
		res = ring_can_read(&pty_to_tty(pty)->i_ring, TTY_IO_BUFF_SZ, 1);
	case POLLOUT:
		res = ring_can_write(&pty_to_tty(pty)->o_ring, TTY_IO_BUFF_SZ, 1);
		break;
	default:
	case POLLERR:
		res = 0;
		break;
	}

	return res;
}

int ppty(int pptyfds[2]) {
	int res;
	struct ppty *ppty;
	struct idesc_ppty *master, *slave;
	struct idesc_table *it;

	it = task_get_idesc_table(task_self());

	ppty = NULL;
	master = slave = NULL;

	ppty = ppty_create();
	if (!ppty) {
		res = ENOMEM;
		goto out_err;
	}

	master = idesc_ppty_create(ppty, &ppty_master_ops, &ppty->master);
	slave = idesc_ppty_create(ppty, &ppty_slave_ops, &ppty->slave);

	if (!master || !slave) {
		res = ENOMEM;
		goto out_err;
	}

	pptyfds[0] = idesc_table_add(it, &master->idesc, 0);
	pptyfds[1] = idesc_table_add(it, &slave->idesc, 0);

	if (pptyfds[0] < 0) {
		res = -pptyfds[0];
		goto out_err;
	}

	if (pptyfds[1] < 0) {
		res = -pptyfds[1];
		goto out_err;
	}

	return 0;

out_err:
	if (pptyfds[1] >= 0) {
		idesc_table_del(it, pptyfds[1]);
	}
	if (pptyfds[0] >= 0) {
		idesc_table_del(it, pptyfds[0]);
	}
	if (master) {
		idesc_ppty_delete(master, &ppty->master);
	}
	if (slave) {
		idesc_ppty_delete(slave, &ppty->slave);
	}
	if (ppty) {
		ppty_delete(ppty);
	}
	SET_ERRNO(res);
	return -1;
}

