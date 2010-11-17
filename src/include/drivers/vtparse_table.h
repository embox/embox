/**
 * @file
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */
#ifndef VTPARSE_TABLE_H_
#define VTPARSE_TABLE_H_

#include <drivers/vt.h>

#define VTPARSE_TRANSITION(action, state) \
		((action) | ((state) << 4))

#define VTPARSE_TRANSITION_ACTION(transition) \
		((transition) & 0x0F)

#define VTPARSE_TRANSITION_STATE(transition) \
		((transition) >> 4)

enum vtparse_state {
	VTPARSE_STATE_ANYWHERE            = 0,
	VTPARSE_STATE_CSI_ENTRY           = 1,
	VTPARSE_STATE_CSI_IGNORE          = 2,
	VTPARSE_STATE_CSI_INTERMEDIATE    = 3,
	VTPARSE_STATE_CSI_PARAM           = 4,
	VTPARSE_STATE_DCS_ENTRY           = 5,
	VTPARSE_STATE_DCS_IGNORE          = 6,
	VTPARSE_STATE_DCS_INTERMEDIATE    = 7,
	VTPARSE_STATE_DCS_PARAM           = 8,
	VTPARSE_STATE_DCS_PASSTHROUGH     = 9,
	VTPARSE_STATE_ESCAPE              = 10,
	VTPARSE_STATE_ESCAPE_INTERMEDIATE = 11,
	VTPARSE_STATE_GROUND              = 12,
	VTPARSE_STATE_OSC_STRING          = 13,
	VTPARSE_STATE_SOS_PM_APC_STRING   = 14
};

typedef enum vtparse_state vtparse_state_t;
typedef enum vtparse_state VTPARSE_STATE;

typedef unsigned char vtparse_state_change_t;
extern const vtparse_state_change_t vtparse_state_table[15][256];
extern const VT_ACTION vtparse_state_entry_actions[15];
extern const VT_ACTION vtparse_state_exit_actions[15];

#endif /* VTPARSE_TABLE_H_ */
