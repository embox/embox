/**
 * @file
 *
 * @date 29.04.14
 * @author Vita Loginova
 */

#ifndef DCACHE_H_
#define DCACHE_H_

struct path;

extern int dcache_add(const char *prefix, const char *rest, struct path *path);
extern struct path *dcache_get(const char *prefix, const char *rest);
extern int dcache_delete(const char *prefix, const char *rest);

#endif /* DCACHE_H_ */
