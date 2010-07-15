/**
 * @file
 * @author Fedor Burdun
 * @data 08.07.2010
 * @description realization of device manipulation
 */

/*
 * TO-DO Fix on -O0/-O2 optimization
 * must work device_read
 */

#include <driver.h>
#include <string.h>
#include <hal/ipl.h>
#include <stdio.h>

#define CONFIG_DEV_MAX_COUNT 0xff

int has_init = 0; 								/* flag of initialization */
device_t device_pool[ CONFIG_DEV_MAX_COUNT ]; 	/* pool of all devices in system */

const char *empty_dev = "empty device";

/* Initialize pool */
void pool_init() {
#if 1
	int i = 0;
	for ( int i=0 ; i < CONFIG_DEV_MAX_COUNT ; ++i ) {
		/* memset( &device_pool[i] , 0 , sizeof( device_t ) ); */
		device_pool[i].desc 	= empty_dev;
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

#if 1
	/* init pool */
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
	/* if pool now is empty */
	if (device_pool[0].private == 0) {
		return NULL;
	}
	/* push from list of empty devices */
	device_t *fr;
	fr = &device_pool[ (device_desc) device_pool[0].private ];
	device_pool[0].private = fr->private;
	/* init some property */
	fr->driver = this;
	fr->desc = name;
	fr->flags = flags;
	fr->private_s = private_s;
	fr->private = kmalloc( private_s );
	/* return same device structure */
	return fr;
#endif
}

int 		device_destroy( device_t *dev ) {
			/* free memory */
#if 1
	/* free memory */
	kfree( dev->private );
	/* set default value */
	dev->desc 		= empty_dev;
	dev->flags		= 0;
	dev->ioc.in		= 0;
	dev->ioc.out	= 0;
	dev->driver 	= NULL;
	dev->private_s= 0;
	/* insert in list of empty */
	dev->private	= device_pool[0].private;
	device_pool[0].private = (void*) (((unsigned long) device_pool -
		(unsigned long ) dev)/(unsigned long)sizeof(device_t));
#endif
}

/* select device by description string (devFS?) */
device_desc device_select( const char *desc ) {
	device_desc i;
	for ( i=0 ; (i<CONFIG_DEV_MAX_COUNT) && (strcmp(desc,device_pool[i].desc)) ; ++i );
	if (!strcmp(desc,device_pool[i].desc)) {
		return i;
	}
	return 0;
}

#if 1
/* shared device's interface */
int device_open  ( device_desc dev , int mode ) {
	/* init pool */
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
	if ( device_pool[dev].desc == empty_dev ) return 0; /* null device :) */
	/* may be must add some check ptr? */
	device_pool[dev].driver->ops.open( &device_pool[dev] , mode );
	return 0;
}

int device_close ( device_desc dev ) {
	/* init pool */
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
	if ( device_pool[dev].desc == empty_dev ) return 0; /* null device :) */
	/* may be must add some check ptr? */
	device_pool[dev].driver->ops.close( &device_pool[dev] );
	return 0;
}

int device_read  ( device_desc dev , char *buf    , size_t n  ) {
	/* init pool */
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
#if 0
	printf("device_read %d %s; empty_dev %s\n",dev,device_pool[dev].desc,empty_dev);
	printf("%d %d\n",&device_pool[dev].desc,&empty_dev);
#endif
	/* if ( dev<0 | dev>=CONFIG_DEV_MAX_COUNT ) return 0; */
	if ( device_pool[dev].desc == empty_dev ) {
		if (0)
		for (--n;n>=0;--n) {
			buf[n]='\0';
		}
		return 0; /* null device :) */
	}
	/* may be must add some check ptr? */
	device_pool[dev].driver->ops.read( &device_pool[dev] , buf , n );
	return 0;
}

int device_write ( device_desc dev , char *buf    , size_t n  ) {
	/* init pool */
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
	/* if ( dev<0 | dev>=CONFIG_DEV_MAX_COUNT ) return 0; */
	if ( device_pool[dev].desc == empty_dev ) return 0; /* null device :) */
	/* may be must add some check ptr? */
	if ( ! (device_pool[dev].driver || device_pool[dev].driver->ops.read )) return 0; /* check like it */

	device_pool[dev].driver->ops.write( &device_pool[dev] , buf , n );
	return 0;
}

int device_ioctl ( device_desc dev , io_cmd c     , void *arg ) {
	/* init pool */
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
	if ( device_pool[dev].desc == empty_dev ) return 0; /* null device :) */
	/* may be must add some check ptr? */
	device_pool[dev].driver->ops.ioctl( &device_pool[dev] , c , arg );
	return 0;
}

int device_devctl( device_desc dev , device_cmd c , void *arg ) {
	/* init pool */
	if (!has_init) {
		pool_init();
		has_init = 1;
	}
	if ( device_pool[dev].desc == empty_dev ) return 0; /* null device :) */
	/* may be must add some check ptr? */
	device_pool[dev].driver->ops.devctl( &device_pool[dev] , c , arg );
	return 0;
}
#endif

