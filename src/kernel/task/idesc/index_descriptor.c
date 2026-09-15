/**
 * @file
 *
 * @date Nov 29, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>

#include <hal/cpu.h>
#include <hal/ipl.h>
#include <kernel/sched.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/thread.h>

#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/idesc.h>

#include <kernel/task/resource/index_descriptor.h>

static inline struct idesc_table *task_self_idesc_table(void) {
	struct idesc_table *it;
	struct thread *th;
	struct task *tk;
	ipl_t ipl;

	/* The table sits at a pointer-aligned offset inside the task's resource
	 * blob, so a misaligned table pointer means the *task* pointer was already
	 * wrong -- and that comes from thread_self()->task. The chain is captured
	 * under a mask and printed whole: without the mask a preemption between
	 * deriving the table and reporting it shows a perfectly healthy thread that
	 * is not the one the table came from. Each link fails for a different
	 * reason -- a stale per-CPU __current_thread, a recycled struct thread, a
	 * clobbered task field -- and from the fault address a caller would see
	 * later they look identical. */
	ipl = ipl_save();
	th = thread_self();
	tk = task_self();
	it = task_resource_idesc_table(tk);
	ipl_restore(ipl);

	assert(it);
	/* Delta is (table - task): the resource offset the kernel actually used.
	 * It is pointer-aligned by construction -- task_resource.c rounds every
	 * offset up to sizeof(void *) and asserts it -- so a misaligned table with
	 * an aligned delta means the task pointer is bad, and a misaligned delta
	 * means the offset is. sizeof(void *), not 8: on a 32-bit target the
	 * resource blob is 4-aligned and an 8-byte demand fires on a healthy
	 * kernel. */
	assertf(!((uintptr_t)it & (sizeof(void *) - 1)),
	    "it %p tk %p res %ld off %ld sz %ld th %p cpu %u", it, tk,
	    (long)((char *)tk->resources - (char *)tk),
	    (long)((char *)it - (char *)tk->resources), (long)TASK_RESOURCE_SIZE,
	    th, cpu_get_id());

	return it;
}

int index_descriptor_add(struct idesc *idesc) {

	return idesc_table_add(task_self_idesc_table(), idesc, 0);
}

struct idesc *index_descriptor_get(int idx) {

	return idesc_table_get(task_self_idesc_table(), idx);
}

int index_descritor_cloexec_get(int fd) {
	struct idesc_table *it;
	int fd_flags = 0;

	it = task_self_idesc_table();
	assert(it);

	if (idesc_is_cloexeced(it->idesc_table[fd])) {
		fd_flags |= FD_CLOEXEC;
	}
	return fd_flags;
}

int index_descriptor_cloexec_set(int fd, int cloexec) {
	struct idesc_table *it;

	it = task_resource_idesc_table(task_self());
	assert(it);

	if (cloexec & FD_CLOEXEC) {
		idesc_cloexec_set(it->idesc_table[fd]);
	} else {
		idesc_cloexec_clear(it->idesc_table[fd]);
	}
	return 0;
}

int index_descriptor_flags_get(int fd) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);
	if (!idesc) {
		return -ENOENT;
	}

	return idesc->idesc_flags & ~O_ACCESS_MASK;
}

#define SETFL_MASK (O_APPEND | O_NONBLOCK | O_NDELAY | O_DIRECT)
int index_descriptor_flags_set(int fd, int flags) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);
	if (!idesc) {
		return -ENOENT;
	}

	idesc->idesc_flags = (flags & SETFL_MASK) | (idesc->idesc_flags & ~SETFL_MASK);

	return 0;
}

int index_descriptor_ioctl(int fd, int req, void *data) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);
	if (!idesc) {
		return -ENOENT;
	}

	assert(idesc->idesc_ops);

	if (!idesc->idesc_ops->ioctl) {
		return -ENOTSUP;
	}
	return idesc->idesc_ops->ioctl(idesc, req, data);

}

int index_descriptor_fcntl(int fd, int cmd, void *data) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);
	if (!idesc) {
		return -ENOENT;
	}

	assert(idesc->idesc_ops);
	if (!idesc->idesc_ops->ioctl) {
		return -ENOTSUP;
	}
	return idesc->idesc_ops->ioctl(idesc, cmd, data);
}

int index_descriptor_status(int fd, int pollmask) {
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);
	if (!idesc) {
		return -ENOENT;
	}

	assert(idesc->idesc_ops);
	if (!idesc->idesc_ops->status) {
		return -ENOTSUP;
	}
	return idesc->idesc_ops->status(idesc, pollmask);
}

int index_descriptor_dupfd(int fd, int newfd) {
	struct idesc_table *it;
	int res;
	struct idesc *idesc;

	idesc = index_descriptor_get(fd);
	if (!idesc) {
		return -ENOENT;
	}

	it = task_self_idesc_table();
	assert(it);

	if (idesc_table_locked(it, newfd)) {
		assert(newfd == 0); /* only for dup() */
		res = idesc_table_add(it, idesc, 0);
	} else {
		res = idesc_table_lock(it, idesc, newfd, 0);
	}

	return res;
}

int index_descriptor_fstat(int fd, struct stat *buff) {
	struct idesc *idesc;

	assert(buff);

	idesc = index_descriptor_get(fd);
	if (!idesc) {
		return -ENOENT;
	}

	assert(idesc->idesc_ops);
	if (!idesc->idesc_ops->fstat) {
		return -ENOTSUP;
	}
	return idesc->idesc_ops->fstat(idesc, buff);
}
