/**
 * @file assert.h
 * @date 25.11.09
 * @author sikmir
 */

#ifndef ASSERT_H_
#define ASSERT_H_

#include "autoconf.h"

#ifdef SIMULATION_TRG
    #define assert(cond)   do ; while(0)
#else
    #define __ASSERT_STRING0(cond, file, line) \
        "\nASSERTION FAILED at " #file " : " #line "\n" \
        "(" cond ") is not TRUE\n"

    #define __ASSERT_STRING(cond, file, line) \
        __ASSERT_STRING0(cond, file, line)

    #define assert(cond) \
        do if (!(cond)) { \
                puts(__ASSERT_STRING(#cond, __FILE__, __LINE__)); \
                HALT; \
        } while(0)
#endif /* SIMULATION_TRG */

#define assert_null(arg) assert((arg) == NULL)
#define assert_not_null(arg) assert((arg) != NULL)

#endif /* ASSERT_H_ */
