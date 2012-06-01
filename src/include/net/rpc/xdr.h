/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @details XDR: External Data Representation Standard, RFC 1832
 * @date 19.05.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_XDR_H_
#define NET_RPC_XDR_H_

#include <stddef.h>
#include <types.h>
#include <net/rpc/rpc.h>

/* Standard size of XDR unit is 4 bytes */
#define BYTES_PER_XDR_UNIT 4
typedef __u32 xdr_unit_t;

#define XDR_SUCCESS 1
#define XDR_FAILURE 0

enum xdr_op {
	XDR_ENCODE,
	XDR_DECODE,
	XDR_FREE
};

struct xdr;

/* XDR Stream options */
struct xdr_ops {
	int (*x_getunit)(struct xdr *xs, xdr_unit_t *to);
	int (*x_putunit)(struct xdr *xs, const xdr_unit_t *from);
	int (*x_getbytes)(struct xdr *xs, char *to, size_t size);
	int (*x_putbytes)(struct xdr *xs, const char *from, size_t size);
	size_t (*x_getpos)(struct xdr *xs);
	int (*x_setpos)(struct xdr *xs, size_t pos);
	char * (*x_inline)(struct xdr *xs, size_t len);
	void (*x_destroy)(struct xdr *xs);
};

struct xdr {
	enum xdr_op oper;
	const struct xdr_ops *ops;
	char *buff;
	char *curr;
	size_t left;
};

typedef int (*xdrproc_t)(struct xdr *, void *, ...);

#define NULL_xdrproc_t NULL
struct xdr_discrim {
	__s32 value;
	xdrproc_t proc;
};

extern void xdrmem_create (struct xdr *xs, char *addr,
		size_t size, enum xdr_op oper);
extern size_t xdr_getpos(struct xdr *xs);
extern int xdr_setpos(struct xdr *xs, size_t pos);
extern char * xdr_inline(struct xdr *xs, size_t len);
extern void xdr_destroy(struct xdr *xs);
extern void xdr_free(xdrproc_t proc, char *obj);

/* XDR filters */
extern int xdr_void(void);
extern int xdr_int(struct xdr *xs, __s32 *pi);
extern int xdr_u_int(struct xdr *xs, __u32 *pui);
extern int xdr_enum(struct xdr *xs, __s32 *pe);
extern int xdr_bool(struct xdr *xs, __s32 *pb);
extern int xdr_opaque(struct xdr *xs, char *pc, size_t size);
extern int xdr_string(struct xdr *xs, char **pstr, __u32 maxsize);
extern int xdr_wrapstring(struct xdr *xs, char **pstr);
extern int xdr_union(struct xdr *xs, __s32 *pdiscriminant, void *punion,
		const struct xdr_discrim *choices, xdrproc_t dfault);

extern int xdr_rpc_msg(struct xdr *xs, struct rpc_msg *msg);

#endif /* NET_RPC_XDR_H_ */
