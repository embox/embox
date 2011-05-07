/**
 * @file
 * @description Interface for use in drivers.
 *
 * @data 08.07.10
 * @author Fedor Burdun
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include <kernel/irq.h>
#include <sys/types.h>
#include <stddef.h>

typedef int device_cmd;
typedef int io_cmd;
typedef int device_desc;
typedef int device_flags;
typedef int driver_flags;

typedef device_desc FILE;
//#define FILE device_desc;

typedef struct device 		device_t;
typedef struct driver 		driver_t;
#ifdef CONFIG_DEV_IO_CONTEXT
typedef struct io_context 	io_context_t;
#endif

/**
 * this interface for char device will not support fseek
 * (call ioctl with some argument)
 * but it may be common interface for all devices
 */
typedef struct device_ops {
	int (*open)  (device_t *dev, int mode);
	int (*close) (device_t *dev);
	int (*read)  (device_t *dev, char *buf, size_t n);
	int (*write) (device_t *dev, char *buf, size_t n);
	int (*ioctl) (device_t *dev, io_cmd c, void *arg);
	int (*devctl)(device_t *dev, device_cmd c, void *arg);
} device_ops_t;

/* return code for -open(), -close(), -read(), -write(), ioctl() */
#define ERROR_IO_UNKNOWN          0x0001
#define ERROR_IO_DONT_SUPPORT     0x0002
#define ERROR_IO_BUFSIZE          0x0003

/* return code for devctl(), device_create(), device_destroy() */
#define ERROR_DEV_UNKNOWN         0x0001
#define ERROR_DEV_DONT_SUPPORT    0x0002
#define ERROR_DEV_BUSY            0x0003

/**
 * Need it or NOT ?
 * io context, that show path to route flow of data
 */
#ifdef CONFIG_DEV_IO_CONTEXT
struct io_context {
	device_desc in, out; /* FILE in, out; */
};
#endif

/**
 * device information, that accesiable for kernel
 */
struct device {
	/* char description of device */
	const char       *desc;
	/* some flag, allows read/write or some operations? */
	device_flags     flags;
#ifdef CONFIG_DEV_IO_CONTEXT
	/* in/out context */
	io_context_t     ioc;
#endif
	/* driver, that controlled its device */
	driver_t         *driver;
	/* description of private data */
	size_t           private_s;
	void             *private;
};

/**
 * description of device's drivers
 * generate by kernel on stage of registry driver in system.
 * Macross EMBOX_DRIVER?
 */
struct driver {
	/* name of driver */
	const char      *name;
	/* interface of device, that controlled by this driver */
	device_ops_t    ops;
	/* some flag of driver */
	driver_flags    flags;
	/* size of private data */
	size_t 	        private_s;
	/* private data */
	void            *private;

	/**
	 * init of driver, allocate memory and other
	 * return 0 or -ERR
	 */
	int (*load)  (driver_t *this);

	/**
	 * find more devices in system and registry they
	 * arg need for send driver some information, for
         * example about path to file for driver of file system
         * in order that create device of specific file
         * return count of new devices or -ERR
         */
	int (*probe) (driver_t *this, void *arg);

	/**
	 * free devices and memory
	 * return 0 or -ERR
	 */
	int (*unload)(driver_t *this);
};

/* function for use in driver */
/* control of devices from driver */

/**
 * allocate memory for device, generate integer descriptor,
 * set some default settings io_context was set by kernel
 * probably with /dev/null
 */
extern device_t *device_create(driver_t *this, const char *name,
			device_flags flags, size_t private_s);

/** free memory */
extern int device_destroy(device_t *dev);

/**
 * select device by description string (devFS?)
 * if can't find device, return /dev/null
 */
extern device_desc device_select(const char *desc);

/* shared device's interface */
extern int device_open  (device_desc dev, int mode);
extern int device_close (device_desc dev);
extern int device_read  (device_desc dev, char *buf, size_t n);
extern int device_write (device_desc dev, char *buf, size_t n);
extern int device_ioctl (device_desc dev, io_cmd c, void *arg);
extern int device_devctl(device_desc dev, device_cmd c, void *arg);

/* IOCTL options */
#define IOCTLR_UNKNOW_OPERATION	0x0001
#define IOCTLR_OK               0x0001

#define IOCTL_SET_BASE_OPTIONS  0x0001
/* arg <=> int* */
#define IOCTL_GET_BASE_OPTIONS  0x0002
/* arg <=> int* */
#define IOCTLP_NONBLOCKINGIO    0x0001

#define DCTL_UNKNOW_OPERATION   0x0001

#endif /* DRIVER_H_ */

