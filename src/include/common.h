#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef _TEST_SYSTEM_
#include "autoconf.h"
#include "types.h"
#include "stdio.h"
#include "err.h"
#include "bitops.h"
#else
#include <linux/types.h>
#include "./../assert.h"
#endif

#if defined(CONFIG_TRACE) && !defined(SIMULATION_TRG)
# ifdef _TEST_SYSTEM_
#  define TRACE(...)  printf(__VA_ARGS__)
# else
#  define TRACE(...)  printk(__VA_ARGS__)
# endif
#else
# define TRACE(...)  do ; while(0)
#endif

#define PRINTREG32_BIN(reg) {int i=0; for(;i<32;i++) TRACE("%d", (reg>>i)&1); TRACE(" (0x%x)\n", reg);}
#define array_len(array)		(sizeof(array) / sizeof(array[0]))

#ifdef _TEST_SYSTEM_
#define REG_LOAD(a)	a
#define REG_STORE(a,v) a=v
#define REG_ORIN(a,v) (REG_STORE(a,(REG_LOAD(a) | (v))))
#define REG_ANDIN(a,v) (REG_STORE(a,(REG_LOAD(a) & (v))))
#else
#define REG_LOAD(a)	(LEON3_BYPASS_LOAD_PA(&(a)))
#define REG_STORE(a,v) (LEON3_BYPASS_STORE_PA(&(a),v))
#define REG_ORIN(a,v) (REG_STORE(a,(REG_LOAD(a) | (v))))
#define REG_ANDIN(a,v) (REG_STORE(a,(REG_LOAD(a) & (v))))
#endif //_TEST_SYSTEM_
#endif //_COMMON_H_
