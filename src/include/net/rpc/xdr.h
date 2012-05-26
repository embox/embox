/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @date 19.05.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_XDR_H_
#define NET_RPC_XDR_H_

#include <stdint.h>

#define XDR_SUCCESS 1
#define XDR_FAILURE 0

enum xdr_op {
	XDR_ENCODE,
	XDR_DECODE,
	XDR_FREE
};

/* XDR Stream options */
struct xdr_ops {
	int (*x_getint)(struct xdr *xs, int *to);
	int (*x_putint)(struct xdr *xs, const int *from);
	int (*x_getbytes)(struct xdr *xs, char *to, unsigned int size);
	int (*x_putbytes)(struct xdr *xs, const char *from, unsigned int size);
	unsigned int (*x_getpos)(struct xdr *xs);
	int (*x_setpos)(struct xdr *xs, unsigned int pos);
	long * (*x_inline)(struct xdr *xs, unsigned int len);
	void (*x_destroy)(struct xdr *xs);
};

struct xdr {
	enum xdr_op oper;
	const struct xdr_ops *ops;
	char *buff;
	char *curr;
	unsigned int left;
};

typedef int (*xdrproc_t)(struct xdr *, void *, ...);

extern void xdrmem_create (struct xdr *xs, char *addr,
		unsigned int size, enum xdr_op oper);
extern unsigned int xdr_getpos(struct xdr *xs);
extern int xdr_setpos(struct xdr *xs, unsigned int pos);
extern int xdr_inline(struct xdr *xs, unsigned int len);
extern void xdr_destroy(struct xdr *xs);
extern void xdr_free(xdrproc_t proc, char *obj);

/* XDR filters */
extern int xdr_void(void);
extern int xdr_int(struct xdr *xs, int *pi);
extern int xdr_string(struct xdr *xs, char **pstr, unsigned int maxsize);
extern int xdr_wrapstring(struct xdr *xs, char **pstr);

#endif /* NET_RPC_XDR_H_ */
