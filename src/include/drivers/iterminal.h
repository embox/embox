/**
 * @file
 * @author Fedor Burdun
 * @date 19.07.2010
 * @brief /dev/iterminal
 *
 * @auther Michail Skorginskii
 * @changes Some desctiption of chanes takes place in src/drivers/iterminal/iterminal.c
 *          Therefore, i add new driver - serial char driver.
 *          I don't know for what we must know time of life, ask Fedir XXX -- rasmikun
 */

#ifndef __ITERMINAL_H_
#define __ITERMINAL_H_

#include <kernel/driver.h>

#define ITERM_DC_SET_IO     0x0101
#define ITERM_DC_SET_IN     0x0102
#define ITERM_DC_SET_OUT    0x0103
#define ITERM_DC_GET_IN     0x0104
#define ITERM_DC_GET_OUT    0x0105

#define ITERM_BUFFER_SIZE 0x100

typedef struct iterminal_private {
    device_desc before_in, before_out; /* for control time of change */
    device_desc in,out;

#if 1 /* under construction */

//    int     is_live;                        /* flag of time of life */
    int     call_state;                     /* state of call (task or cmdline edit) */

    char    buffer_in[ITERM_BUFFER_SIZE];   /* buffer for read from input device */
    int     buffer_in_s;                    /* size of input buffer */

    char    buffer_out[ITERM_BUFFER_SIZE];  /* --/--/-- */
    int     buffer_out_s;                   /* --/--/-- */

    device_desc pipe_to, pipe_from; /* some pipes for exchange data with child task */

#endif
} iterminal_private_t;


typedef struct serial_private {
    /* privae data for serial driver */
}

#endif /* __ITERMINAL_H_ */
