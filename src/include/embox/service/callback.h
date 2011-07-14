/**
 * @file Callback body macroses
 * @brief
 *
 *
 * @date 7.07.11
 * @author Anton Kozlov
 */

#ifndef EMBOX_SERVICE_CALLBACK_H_
#define EMBOX_SERVICE_CALLBACH_H_

#include <framework/service/types.h>

#define CALLBACK_INIT(name)  \
    extern const struct callback_mng __callback_registry[];\
    callback_t name##_callback;\
    static void name##_callback_reg(callback_t callback) { \
	name##_callback = callback;\
    }\
    static int name##_callback_do(int msg, void *data) { \
	return name##_callback(msg, data);\
    }\
    ARRAY_SPREAD_ADD_NAMED(__callback_registry, callback_mng, {\
	    .reg = (callback_reg_t) name##_callback_reg,\
	    .cb_do = (callback_t) name##_callback_do\
	    })

#define CALLBACK_DECLARE(name) \
    extern callback_t name##_callback

#define CALLBACK_REG(name, callback) \
    do { name##_callback = callback; } while (0)

#define CALLBACK_DO(name, msg, data)\
    name##_callback_do( msg, (void *) data)

#endif /* EMBOX_SERVICE_CALLBACK_H_ */

