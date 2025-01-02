#include <syslog.h>

#include "iec_std_lib.h"

typedef struct {
	__DECLARE_VAR(BOOL, EN)
	__DECLARE_VAR(BOOL, ENO)
	__DECLARE_VAR(STRING, MSG)
	__DECLARE_VAR(UINT, LVL)
	__DECLARE_VAR(BOOL, SUCCESS)
} LOGGER;

static inline void LOGGER_init__(LOGGER *data__, BOOL retain) {
	__INIT_VAR(data__->EN, TRUE, retain)
	__INIT_VAR(data__->MSG, __STRING_LITERAL(0, ""), retain)
	__INIT_VAR(data__->LVL, 0, retain)
	__INIT_VAR(data__->SUCCESS, FALSE, retain)
}

static inline void LOGGER_body__(LOGGER *data__) {
	int err;

	if (!__GET_VAR(data__->EN)) {
		__SET_VAR(data__->, ENO, , FALSE);
		return;
	}

	__SET_VAR(data__->, ENO, , TRUE);

	syslog(__GET_VAR(data__->LVL), __GET_VAR(data__->MSG).body);

	__SET_VAR(data__->, SUCCESS, , TRUE);
}
