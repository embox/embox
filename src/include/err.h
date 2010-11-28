/**
 * @file
 *
 * @date 25.11.09
 * @author Nikolay Korotky
 */
#ifndef ERR_H_
#define ERR_H_

#include <stdio.h>

#define LOGGER()   printf("%s (%s:%d) ", __FUNCTION__, __FILE__, __LINE__)

#if defined(CONFIG_ERROR)
# define LOG_ERROR(...)  do {LOGGER(); printf("ERROR: "__VA_ARGS__);} while(0)
#else
# define LOG_ERROR(...)  do ; while(0)
#endif

#if defined(CONFIG_WARN)
# define LOG_WARN(...)  do {LOGGER();  printf("WARN: "__VA_ARGS__);} while(0)
#else
# define LOG_WARN(...)  do ; while(0)
#endif

#if defined(CONFIG_DEBUG)
# define LOG_DEBUG(...)  do {LOGGER(); printf("DEBUG: "__VA_ARGS__);} while(0)
#else
# define LOG_DEBUG(...)  do ; while(0)
#endif

#endif /* ERR_H_ */
