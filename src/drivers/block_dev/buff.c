/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Andrey Gazukin
 */

#include <types.h>
#include <errno.h>
#include <embox/block_dev.h>
#include <embox/buff.h>
#include <mem/page.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <mem/phymem.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)

POOL_DEF(buff_pool, struct buf, MAX_DEV_QUANTITY);

buf_t *init_buffer_pool(dev_t devno, int blocks) {
	int pagecnt;
	block_dev_t *dev;
	buf_t *buf;

	if(NULL == (dev = device(devno))) {
		return NULL;
	}

	if (NULL == dev->buff) {
		buf = (buf_t *) pool_alloc(&buff_pool);
		dev->buff = buf;
	}
	else {
		buf = dev->buff;
	}
	buf->lastblkno = -1;
	buf->buff_cntr = -1;

	if(0 >= (buf->blksize =
			dev_ioctl(devno, IOCTL_GETBLKSIZE, NULL, 0))) {
		return NULL;
	}

	pagecnt = 1;
	if(buf->blksize > PAGE_SIZE()) {
		pagecnt = buf->blksize / PAGE_SIZE();
		if(buf->blksize % PAGE_SIZE()) {
			pagecnt++;
		}
	}
	buf->blkfactor = pagecnt;

	if(NULL == (buf->pool = page_alloc(__phymem_allocator, blocks * pagecnt))) {
		return NULL;
	}
	buf->depth = blocks;

	return  buf;
}

buf_t *get_buffer(dev_t devno, blkno_t blkno) {
	buf_t *buf;

	if(NULL == (buf = (device(devno)->buff))) {
		return NULL;
	}

	/* set pointer to the buffer in pool */
	if(buf->lastblkno != blkno) {
		buf->buff_cntr++;
		buf->buff_cntr %= buf->depth;

		buf->data = buf->pool + buf->buff_cntr * PAGE_SIZE() * buf->blkfactor;
		buf->blkno = blkno;

		dev_read(devno, buf->data, buf->blksize, buf->blkno);
		buf->lastblkno = blkno;
	}

	return buf;
}

int free_buffer_pool(dev_t devno) {
	block_dev_t *dev;
	buf_t *buf;

	if(NULL == (dev = device(devno))) {
		return -1;
	}

	if (NULL == dev->buff) {
		return 0;
	}
	else {
		buf = dev->buff;
	}

	page_free(__phymem_allocator, buf->pool, buf->depth * buf->blkfactor);
	pool_free(&buff_pool, buf);

	return  0;
}
