/**
 * \file amba_drivers_helper.h
 *
 * \date Jun 3, 2009
 * \author Anton Bondarev
 * \details must include only after definition dev_regs (pointer to struct registers). it contains macroses for hepling to write drivers module
 */

#ifndef AMBA_DRIVERS_HELPER_H_
#define AMBA_DRIVERS_HELPER_H_



#ifdef _TEST_SYSTEM_
static AMBA_DEV amba_dev;
#endif

inline static int module_is_inited()
{
	return (int)dev_regs;
}

#define CHECK_INIT_MODULE() if (!module_is_inited()){module_init();}
#endif /* AMBA_DRIVERS_HELPER_H_ */
