/**
 * @file
 * @brief Realization of the eXternal Data Representation
 * @date 21.05.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <net/rpc/xdr.h>
#include <string.h>

/* Useful routines */
static int xdr_getint(struct xdr *xs, int *to) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_getint != NULL);

	return (*xs->ops->x_getint)(xs, to);
}

static int xdr_putint(struct xdr *xs, const int *from) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_putint != NULL);

	return (*xs->ops->x_putint)(xs, from);
}

static int xdr_getbytes(struct xdr *xs, char *to, unsigned int size) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_getbytes != NULL);

	return (*xs->ops->x_getbytes)(xs, to, size);
}

static int xdr_putbytes(struct xdr *xs, const char *from, unsigned int size) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_putbytes != NULL);

	return (*xs->ops->x_putbytes)(xs, from, size);
}

void xdr_free(xdrproc_t proc, char *obj) {
	struct xdr tmp;

	assert(proc != NULL);

	tmp.oper = XDR_FREE;
	(*proc)(&tmp, obj);
}

int xdr_void(struct xdr *xs, void *unused) {
	assert(xs != NULL);

	return XDR_SUCCESS;
}

int xdr_int(struct xdr *xs, int *pi) {
	assert(xs != NULL);

	switch (xs->oper) {
	case XDR_DECODE:
		return xdr_getint(xs, pi);
	case XDR_ENCODE:
		return xdr_putint(xs, pi);
	case XDR_FREE:
		return XDR_SUCCESS;
	}

	return XDR_FAILURE; /* unknown operation */
}

int xdr_string(struct xdr *xs, char **pstr, unsigned int maxsize) {
	char *str;
	unsigned int size;

	assert((xs != NULL) && (pstr != NULL));

	str = *pstr;
	switch (xs->oper) {
	case XDR_DECODE:
	case XDR_ENCODE:
	case XDR_FREE:
		if (str == NULL) {
			return XDR_SUCCESS;
		}
		size = strlen(str);
		break;
	}

	return XDR_FAILURE;
}

int xdr_wrapstring(struct xdr *xs, char **pstr) {
	return xdr_string(xs, pstr, ((unsigned int)-1));
}

unsigned int xdr_getpos(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_getpos != NULL);

	return (*xs->ops->x_getpos)(xs);
}

int xdr_setpostn(struct xdr *xs, unsigned int pos) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_setpos != NULL);

	return (*xs->ops->x_setpos)(xs, pos);
}

long * xdr_inline(struct xdr *xs, unsigned int len) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_inline != NULL);

	return (*xs->ops->x_inline)(xs, len);
}

void xdr_destroy(struct xdr *xs) {
	assert(xs != NULL);
	assert(xs->ops != NULL);
	assert(xs->ops->x_destroy != NULL);

	if (xs->ops->x_destroy) {
		(*xs->ops->x_destroy)(xs);
	}
}
