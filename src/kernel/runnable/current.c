/**
 * @file
 * @brief
 *
 * @date 28.07.14
 * @author Vita Loginova
 */

#include <kernel/cpu/cpudata.h>

struct runnable;

/* Defining current thread for each CPU */
struct runnable *__current_runnable __cpudata__;
