/**
 * @file
 * @brief
 *
 * @date 28.07.14
 * @author Vita Loginova
 */

#include <kernel/cpu/cpudata.h>

struct schedee;

/* Defining current schedee for each CPU */
struct schedee *__current_schedee __cpudata__;
