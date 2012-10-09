/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Andrey Gazukin
 */


#ifndef BUFF_H_
#define BUFF_H_

typedef struct buf {
	blkno_t blkno;
	blkno_t lastblkno;
	char *data;
	int blksize;
	int depth;
	char *pool;
	int buff_cntr;
} buf_t;

extern buf_t *init_buffer_pool(dev_t devno, int blocks);
extern buf_t *get_buffer(dev_t devno, blkno_t blkno);

#endif /* BUFF_H_ */
