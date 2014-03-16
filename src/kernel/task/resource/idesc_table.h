/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 25.02.14
 */

#ifndef IDESC_TABLE_H_
#define IDESC_TABLE_H_

#include <config/embox/kernel/task/resource/idesc_table.h>
#include <framework/mod/options.h>
#include <kernel/task/resource/idesc_table.h>
#include <util/indexator.h>

#define MODOPS_IDESC_TABLE_SIZE \
	OPTION_MODULE_GET(embox__kernel__task__resource__idesc_table, \
			NUMBER, idesc_table_size)

struct idesc;

struct idesc_table {
	struct idesc *idesc_table[MODOPS_IDESC_TABLE_SIZE];

	struct indexator indexator;
	index_data_t index_buffer[INDEX_DATA_LEN(MODOPS_IDESC_TABLE_SIZE)];
};

#endif /* IDESC_TABLE_H_ */
