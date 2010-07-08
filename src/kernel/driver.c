/**
 * @file
 * @author Fedor Burdun
 * @data 08.07.2010
 * @description realization of device manipulation
 */

#include <driver.h>
//#include <string.h>

#define CONFIG_DEV_MAX_COUNT 0xff

int has_init = 0; 								/* flag of initialization */
device_t device_pool[ CONFIG_DEV_MAX_COUNT ]; 	/* pool of all devices in system */

const char empty_dev = "empty device";

/* Initialize pool */
void pool_init() {
#if 0
	int i;
	for ( int i=0 ; i < CONFIG_DEV_MAX_COUNT ; ++i ) {
		/* memset( &device_pool[i] , 0 , sizeof( device_t ) ); */
		device_pool[i].name 	= empty_dev;
		device_pool[i].flags	= 0;
		device_pool[i].ioc.in	= 0;
		device_pool[i].ioc.out	= 0;
		device_pool[i].driver 	= NULL;
		device_pool[i].private_s= 0;
		device_pool[i].private	= NULL;
	}

	for ( int i=1 ; i< CONFIG_DEV_MAX_COUNT ; ++i ) {
		device_pool[i-1].private = i; /* WARINIG! In empty dev private ptr use as index of next
			empty device in pool */
	}
#endif
}

device_t 	*device_create( driver_t *this , const char *name , device_flags flags , size_t private_s )
			/* allocate memory for device, generate integer descriptor, set some default settings */
			/* io_context was set by kernel probably with /dev/null */ {

#if 0
	if (device_pool[0].private == 0) {
		return NULL;
	}
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
#endif
}

int 		device_destroy( device_t *dev ) {
			/* free memory */

}

