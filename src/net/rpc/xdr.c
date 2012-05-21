/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdint.h>
#include <net/rpc/xdr.h>

static int xdr_getint32(struct xdr *xs, __s32 *to) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_getint32 != NULL);
	return (*xs->x_ops->x_getint32)(xs, to);
}

static int xdr_putint32(struct xdr *xs, const __s32 *from) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_putint32 != NULL);
	return (*xs->x_ops->x_putint32)(xs, from);
}

static int xdr_getlong(struct xdr *xs, long *to) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_getlong != NULL);
	return (*xs->x_ops->x_getlong)(xs, to);
}

static int xdr_putlong(struct xdr *xs, const long *from) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_putlong != NULL);
	return (*xs->x_ops->x_putlong)(xs, from);
}

static int xdr_getbytes(struct xdr *xs, char *to, unsigned int size) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_getbytes != NULL);
	return (*xs->x_ops->x_getbytes)(xs, to, size);
}

static int xdr_putbytes(struct xdr *xs, const char *from, unsigned int size) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_putbytes != NULL);
	return (*xs->x_ops->x_putbytes)(xs, from, size);
}

static unsigned int xdr_getpostn(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_getpostn != NULL);
	return (*xs->x_ops->x_getpostn)(xs);
}

static int xdr_setpostn(struct xdr *xs, unsigned int pos) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_setpostn != NULL);
	return (*xs->x_ops->x_setpostn)(xs, pos);
}

static int xdr_inline(struct xdr *xs, unsigned int len) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_inline != NULL);
	return (*xs->x_ops->x_inline)(xs, len);
}

static void xdr_destroy(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->x_ops != NULL);
	assert(xs->x_ops->x_destroy != NULL);
	if (xs->x_ops->x_destroy) {
		(*xs->x_ops->x_destroy)(xs);
	}
}

void xdrmem_create (struct xdr *xdrs, const char *addr,
		unsigned int size, enum xdr_op xop) {
	;
}

void xdr_free(xdrproc_t proc, char *obj) {
	;
}

int xdr_void(void) {
	;
}

int xdr_int(struct xdr *xdrs, int *pi) {
	;
}

int xdr_string(struct xdr *xdrs, char **ppc, unsigned int maxsize) {
	;
}

int xdr_wrapstring(struct xdr *xdrs, char **ppc) {
	;
}
