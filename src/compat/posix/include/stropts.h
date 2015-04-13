/*
 *
 * @date 3 Oct 2014
 * @authro Denis Deryugin
 */

#ifndef _STROPTS_H_
#define _STROPTS_H_

/* Macros used as "request" in ioctl() */
#define I_PUSH	0x1
#define I_POP	0x2
#define I_LOOK	0x3
#define I_FLUSH	0x4

/* Flush options. */
#define FLUSHR	0x01	/* Flush read queues. */
#define FLUSHW	0x02	/* Flush write queues. */
#define FLUSHRW	0x03	/* Flush read and write queues. */

#endif
