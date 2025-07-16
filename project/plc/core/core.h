/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 23.12.24
 */

#ifndef PLC_CORE_H_
#define PLC_CORE_H_

#include <stdint.h>
#include <sys/cdefs.h>
#include <time.h>

#include <kernel/time/timer.h>
#include <lib/libds/array.h>

#define PLC_PHYS_VAR_QX  0 /* Digital Outputs */
#define PLC_PHYS_VAR_IX  1 /* Digital Inputs */
#define PLC_PHYS_VAR_IW  2 /* Analog Inputs */
#define PLC_PHYS_VAR_QW  3 /* Analog Outputs */
#define PLC_SLAVE_VAR_QX 4 /* Coils */
#define PLC_SLAVE_VAR_IX 5 /* Contacts */
#define PLC_SLAVE_VAR_IW 6 /* Input Registers */
#define PLC_SLAVE_VAR_MW 7 /* Holding Registers */

#define PLC_CONFIG_REGISTER(config_ptr) \
	ARRAY_SPREAD_ADD(__plc_config_registry, config_ptr)

struct plc_phys_var {
	void *data;
	uint16_t type;
	uint16_t port;
	uint16_t pin;
	uint16_t alt;
};
struct plc_slave_var {
	void *data;
	uint16_t type;
	uint16_t addr;
};

struct plc_config_ops {
	void (*init)(void);
	void (*run)(unsigned long tick);
};

struct plc_config {
	const char *name;
	const struct plc_phys_var *phys_vars;
	const struct plc_slave_var *slave_vars;
	unsigned long long *common_ticktime; // nsec
	unsigned long *greatest_tick_count;  // tick
	struct plc_config_ops ops;
};

__BEGIN_DECLS

extern int plc_start(const struct plc_config *config);
extern int plc_stop(void);

extern void plc_slave_handle(void (*handler)(const struct plc_slave_var *));

__END_DECLS

ARRAY_SPREAD_DECLARE(const struct plc_config *const, __plc_config_registry);

#define plc_config_foreach(config) \
	array_spread_foreach(config, __plc_config_registry)

#define plc_phys_var_foreach(var, config) \
	for (var = config->phys_vars; var->data; var++)

#define plc_slave_var_foreach(var, config) \
	for (var = config->slave_vars; var->data; var++)

#endif /* PLC_CORE_H_ */
