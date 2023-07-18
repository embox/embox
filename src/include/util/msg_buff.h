/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.07.23
 */
#ifndef UTIL_MSG_BUFF_H_
#define UTIL_MSG_BUFF_H_

#include <stddef.h>
#include <sys/cdefs.h>

#include <util/ring_buff.h>

struct msg_buff {
	struct ring_buff rbuf;
};

__BEGIN_DECLS

extern void msg_buff_init(struct msg_buff *buf, void *storage, size_t size);

extern size_t msg_buff_enqueue(struct msg_buff *buf, const void *src,
    size_t nbyte);

extern size_t msg_buff_dequeue(struct msg_buff *buf, void *dst, size_t nbyte);

extern size_t msg_buff_space(struct msg_buff *buf);

extern int msg_buff_empty(struct msg_buff *buf);

__END_DECLS

#endif /* UTIL_MSG_BUFF_H_ */
