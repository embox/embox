/**
 * @brief Common interface for flash subsystem
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */
#include <errno.h>
#include <errno.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>

#include <util/math.h>

struct flash_block_info *
flash_block_info_by_block(struct flash_dev *flashdev, int block) {
	int i;
	int cur = 0;

	for (i = 0; i < flashdev->num_block_infos; i ++) {
		if (cur <= block && block < (cur + flashdev->block_info[i].blocks) ) {
			return &flashdev->block_info[i];
		}
		cur += flashdev->block_info[i].blocks;
	}

	return NULL;
}

int
flash_get_block_size(struct flash_dev *flashdev, int block) {
	int i;
	int cur = 0;

	for (i = 0; i < flashdev->num_block_infos; i ++) {
		if (cur <= block && block < (cur + flashdev->block_info[i].blocks) ) {
			return flashdev->block_info[i].block_size;
		}
		cur += flashdev->block_info[i].blocks;
	}

	return -EINVAL;
}

int
flash_get_block_by_offset(struct flash_dev *flashdev, unsigned long offset) {
	int i, j;
	int bl = 0;
	unsigned long cur = 0;

	for (i = 0; i < flashdev->num_block_infos; i++) {
		for (j = 0; j < flashdev->block_info[i].blocks; j++) {
			if (cur <= offset && 
						offset < (cur + flashdev->block_info[i].block_size) ) {
				return bl;
			}
			cur += flashdev->block_info[i].block_size;
			bl++;
		}
	}

	return -EINVAL;
}

int
flash_get_offset_by_block(struct flash_dev *flashdev, int block) {
	int i, j;
	int bl = 0;
	unsigned long cur = 0;

	for (i = 0; i < flashdev->num_block_infos; i++) {
		for (j = 0; j < flashdev->block_info[i].blocks; j++) {
			if (bl <= block &&
						block < (bl + flashdev->block_info[i].blocks) ) {
				return (int) cur;
			}
			cur += flashdev->block_info[i].block_size;
			bl++;
		}
	}

	return -EINVAL;
}

int
flash_get_blocks_num(struct flash_dev *flashdev) {
	int i;
	int num = 0;

	for (i = 0; i < flashdev->num_block_infos; i ++) {
		num += flashdev->block_info[i].blocks;
	}

	return num;
}

/* Handlers to check ranges and call device-specific functions */
int flash_read(struct flash_dev *flashdev, unsigned long offset, void *buf,
    size_t len) {
	assert(buf);
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_read);

	assert(offset + len <= flashdev->size);

	return flashdev->drv->flash_read(flashdev, offset, buf, len);
}

int flash_write(struct flash_dev *flashdev, unsigned long offset,
    const void *buf, size_t len) {
	assert(buf);
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_program);

	assert(offset + len <= flashdev->size);

	return flashdev->drv->flash_program(flashdev, offset, buf, len);
}

int flash_erase(struct flash_dev *flashdev, uint32_t block) {
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_erase_block);

	return flashdev->drv->flash_erase_block(flashdev, block);
}

struct flash_dev *flash_by_bdev(struct block_dev *bdev) {
	return bdev->dev_module.dev_priv;
}


int flash_read_aligned(struct flash_dev *flashdev, unsigned long offset,
    void *buff, size_t len) {
	int i;
	char *b;
	uint32_t word32;
	int head;
	int word_size;

	assert(buff);

	if (flashdev->fld_aligned_word) {
		b = flashdev->fld_aligned_word;
		word_size = flashdev->fld_word_size;
	}
	else {
		b = (void *)&word32;
		word_size = sizeof(word32);
	}

	head = offset % word_size;

	if (head) {
		size_t head_cnt = min(len, word_size - head);

		offset -= head;
		flash_read(flashdev, offset, b, word_size);
		memcpy(buff, b + head, head_cnt);

		if (len <= head_cnt) {
			return 0;
		}

		buff += head_cnt;
		offset += word_size;
		len -= head_cnt;
	}

	for (i = 0; len >= word_size; i++) {
		flash_read(flashdev, offset, b, word_size);
		memcpy(buff, b, word_size);

		offset += word_size;
		buff += word_size;
		len -= word_size;
	}

	if (len > 0) {
		flash_read(flashdev, offset, b, word_size);
		memcpy(buff, b, len);
	}

	return 0;
}

/* @brief Write non-aligned raw data to \b erased NAND flash
 * @param offset Start position on disk
 * @param buff   Source of the data
 * @param len    Length of the data in bytes
 *
 * @returns Bytes written or negative error code
 */
int flash_write_aligned(struct flash_dev *flashdev, unsigned long offset,
    const void *buff, size_t len) {
	int i;
	char *b;
	uint32_t word32;
	int head;
	int word_size;

	assert(buff);

	if (flashdev->fld_aligned_word) {
		b = flashdev->fld_aligned_word;
		word_size = flashdev->fld_word_size;
	}
	else {
		b = (void *)&word32;
		word_size = sizeof(word32);
	}

	head = offset % word_size;

	if (head) {
		size_t head_write_cnt = min(len, word_size - head);

		offset -= head;
		flash_read(flashdev, offset, b, word_size);
		memcpy(b + head, buff, head_write_cnt);
		flash_write(flashdev, offset, b, word_size);

		if (len <= head_write_cnt) {
			return 0;
		}

		buff += head_write_cnt;
		offset += word_size;
		len -= head_write_cnt;
	}

	for (i = 0; len >= word_size; i++) {
		memcpy(b, buff, word_size);
		flash_write(flashdev, offset, b, word_size);

		offset += word_size;
		buff += word_size;
		len -= word_size;
	}

	if (len > 0) {
		flash_read(flashdev, offset, b, word_size);
		memcpy(b, buff, len);
		flash_write(flashdev, offset, b, word_size);
	}

	return 0;
}

int flash_copy_aligned(struct flash_dev *flashdev, unsigned long to,
    unsigned long from, int len) {
	char b[32];

	while (len > 0) {
		int tmp_len;

		tmp_len = min(len, sizeof(b));

		if (0 > flash_read_aligned(flashdev, from, b, tmp_len)) {
			return -1;
		}
		if (0 > flash_write_aligned(flashdev, to, b, tmp_len)) {
			return -1;
		}

		len -= tmp_len;
		to += tmp_len;
		from += tmp_len;
	}

	return 0;
}

int flash_copy_block(struct flash_dev *flashdev, unsigned int to,
    unsigned long from) {
	uint32_t block_size;
	struct flash_block_info *to_fi = flash_block_info_by_block(flashdev, to);

	//block_size = flashdev->block_info[0].block_size;
	block_size = to_fi->block_size;

	flash_erase(flashdev, to);

	return flash_copy_aligned(flashdev, to * block_size, from * block_size,
	    block_size);
}
