/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @date 19.05.12
 * @author Ilia Vaprol
 */

#ifndef NET_RPC_XDR_H_
#define NET_RPC_XDR_H_

#include <stdint.h>

enum xdr_op {
	XDR_ENCODE,
	XDR_DECODE,
	XDR_FREE
};

/* XDR Stream options */
struct xdr_ops {
	int (*x_getint32)(struct xdr *xs, __s32 *to);
	int (*x_putint32)(struct xdr *xs, const __s32 *from);
	int (*x_getlong)(struct xdr *xs, long *to);
	int (*x_putlong)(struct xdr *xs, const long *from);
	int (*x_getbytes)(struct xdr *xs, char *to, unsigned int size);
	int (*x_putbytes)(struct xdr *xs, const char *from, unsigned int size);
	unsigned int (*x_getpostn)(struct xdr *xs);
	int (*x_setpostn)(struct xdr *xs, unsigned int pos);
	int (*x_inline)(struct xdr *xs, unsigned int len);
	void (*x_destroy)(struct xdr *xs);
};

struct xdr {
	enum xdr_op x_op;
	struct xdr_ops *x_ops;
};

typedef int (*xdrproc_t)(struct xdr *, void *, ...);

extern void xdrmem_create (struct xdr *xs, const char *addr,
		unsigned int size, enum xdr_op xop);

extern int xdr_void(void);
extern int xdr_int(struct xdr *xs, int *pi);
extern int xdr_string(struct xdr *xs, char **ppc, unsigned int maxsize);
extern int xdr_wrapstring(struct xdr *xs, char **ppc);

extern void xdr_free(xdrproc_t proc, char *obj);

#endif /* NET_RPC_XDR_H_ */
