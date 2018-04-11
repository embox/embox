#ifndef XEN_XENSTORE_H_
#define XEN_XENSTORE_H_

#include <stdint.h>
#include <xen/xen.h>
#include <errno.h>
#include <xen/io/xs_wire.h>

#if defined(__i386__)
#include <xen_hypercall-x86_32.h>
#elif defined(__x86_64__)
#include <xen_hypercall-x86_64.h>
#else
#error "Unsupported architecture"
#endif

int xenstore_init(start_info_t *start);
int xenstore_write(char *key, char *value);
int xenstore_read(char *key, char *value, int value_length);
int xenstore_ls(char *key, char *values, int value_length);

#endif /* XEN_XENSTORE_H_ */
