/**
 * @file
 *
 * @details http://pubs.opengroup.org/onlinepubs/007908775/xsh/sysconf.html
 *
 * @date Oct 17, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

#include <mem/page.h>
#include <hal/cpu.h>

long int sysconf(int name) {
	switch(name) {
	case _SC_PAGESIZE:
		return PAGE_SIZE();
	case _SC_CLK_TCK:
		return CLK_TCK;
	case _SC_NPROCESSORS_ONLN:
	//http://www.gnu.org/software/libc/manual/html_node/Processor-Resources.html
		return NCPU;
	case _SC_GETPW_R_SIZE_MAX:
		return 0x200;
	case _SC_ATEXIT_MAX:
		return ATEXIT_MAX;
	case _SC_PHYS_PAGES:
		//FIXME
		return 0x1000;
	default:
		return -EINVAL;
	}
}
