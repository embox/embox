/**
 * @file
 *
 * @date Oct 17, 2013
 * @author: Anton Bondarev
 */

#include <mem/page.h>

#include <unistd.h>

int getpagesize(void) {
	return (int)PAGE_SIZE();
}
