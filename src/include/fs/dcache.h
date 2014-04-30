/*
 * dcache.h
 *
 *  Created on: Apr 29, 2014
 *      Author: vita
 */

#ifndef DCACHE_H_
#define DCACHE_H_

#include <fs/path.h>

extern int dcache_add(const char *fullpath, struct path *path);
extern struct path *dcache_get(const char *fullpath);

#endif /* DCACHE_H_ */
