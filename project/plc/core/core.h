/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 23.12.24
 */

#ifndef PLC_CORE_H_
#define PLC_CORE_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

#define PLC_LOCATED_QX 0
#define PLC_LOCATED_QW 1
#define PLC_LOCATED_IX 2
#define PLC_LOCATED_IW 3
#define PLC_LOCATED_MX 4
#define PLC_LOCATED_MW 5
#define PLC_LOCATED_MD 6
#define PLC_LOCATED_ML 7

struct plc_located_var {
	void *addr;
	uint16_t type;
	uint16_t n0;
	uint16_t n1;
	uint16_t n2;
	uint16_t n3;
};

__BEGIN_DECLS

extern bool plc_is_started(void);

extern int plc_start(void);

extern int plc_stop(void);

extern struct plc_located_var __plc_located_var_list[];

__END_DECLS

#define plc_located_var_foreach(var) \
	for (var = __plc_located_var_list; var->addr; var++)

#endif /* PLC_CORE_H_ */
