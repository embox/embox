/**
 * @file
 * @brief
 *
 * @date 07.07.2011
 * @author Anton Kozlov
 */

#ifndef FRAMEWORK_SERVICE_TYPES_H_
#define FRAMEWORK_SERVICE_TYPES_H_

typedef int (*callback_t) (int msg, void *data);
typedef int (*callback_reg_t) (callback_t callback);

struct callback_mng {
	callback_reg_t reg;
	callback_t cb_do;
};

#endif /* FRAMEWORK_SERVICE_TYPES_H_ */
