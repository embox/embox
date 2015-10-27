/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-27
 */

#ifndef DRIVERS_BLOCK_DEV_PARTITION_H_
#define DRIVERS_BLOCK_DEV_PARTITION_H_

struct hd;
extern int create_partitions(struct hd *hd);

#endif /* DRIVERS_BLOCK_DEV_PARTITION_H_ */
