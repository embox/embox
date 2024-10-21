#include <lib/leddrv.h>

#include "iec_std_lib.h"

typedef struct {
	__DECLARE_VAR(BOOL, EN)
	__DECLARE_VAR(BOOL, ENO)
	__DECLARE_VAR(UINT, LED_NUM)
	__DECLARE_VAR(BOOL, SUCCESS)
} LEDDRV_SET;

static inline void LEDDRV_SET_init__(LEDDRV_SET *data__, BOOL retain) {
	__INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
	__INIT_VAR(data__->LED_NUM, __UINT_LITERAL(0), retain)
	__INIT_VAR(data__->SUCCESS, __BOOL_LITERAL(FALSE), retain)
}

static inline void LEDDRV_SET_body__(LEDDRV_SET *data__) {
	int err;

	if (!__GET_VAR(data__->EN)) {
		__SET_VAR(data__->, ENO, , __BOOL_LITERAL(FALSE));
		return;
	}

	__SET_VAR(data__->, ENO, , __BOOL_LITERAL(TRUE));

	err = leddrv_set(__GET_VAR(data__->LED_NUM));

	__SET_VAR(data__->, SUCCESS, ,
	    err ? __BOOL_LITERAL(FALSE) : __BOOL_LITERAL(TRUE));
}

typedef struct {
	__DECLARE_VAR(BOOL, EN)
	__DECLARE_VAR(BOOL, ENO)
	__DECLARE_VAR(UINT, LED_NUM)
	__DECLARE_VAR(BOOL, SUCCESS)
} LEDDRV_CLR;

static inline void LEDDRV_CLR_init__(LEDDRV_CLR *data__, BOOL retain) {
	__INIT_VAR(data__->EN, __BOOL_LITERAL(TRUE), retain)
	__INIT_VAR(data__->LED_NUM, __UINT_LITERAL(0), retain)
	__INIT_VAR(data__->SUCCESS, __BOOL_LITERAL(FALSE), retain)
}

static inline void LEDDRV_CLR_body__(LEDDRV_CLR *data__) {
	int err;

	if (!__GET_VAR(data__->EN)) {
		__SET_VAR(data__->, ENO, , __BOOL_LITERAL(FALSE));
		return;
	}

	__SET_VAR(data__->, ENO, , __BOOL_LITERAL(TRUE));

	err = leddrv_clr(__GET_VAR(data__->LED_NUM));

	__SET_VAR(data__->, SUCCESS, ,
	    err ? __BOOL_LITERAL(FALSE) : __BOOL_LITERAL(TRUE));
}
