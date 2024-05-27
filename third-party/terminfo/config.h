/**
 * @brief
 *
 * @date 27.05.24
 * @author Aleksey Zhmulin
 */

#ifndef TERMINFO_CONFIG_H_
#define TERMINFO_CONFIG_H_

#include <framework/mod/options.h>

#include <config/third_party/terminfo.h>

#define TERMINFO_TERM OPTION_MODULE_STRING_GET(third_party__terminfo, term)
#define TERMINFO_PATH OPTION_MODULE_STRING_GET(third_party__terminfo, path)

#endif /* TERMINFO_CONFIG_H_ */
