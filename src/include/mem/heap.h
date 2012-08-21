/**
 * @file
 *
 * @brief
 *
 * @date 12.07.2012
 * @author Anton Bondarev
 */

#ifndef MEM_HEAP_H_
#define MEM_HEAP_H_
#include <framework/mod/options.h>

#include <module/embox/mem/heap_api.h>

#define HEAP_SIZE() OPTION_MODULE_GET(embox__mem__heap_api,NUMBER,heap_size)

#endif /* MEM_HEAP_H_ */
