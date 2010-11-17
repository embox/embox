/**
 * @file
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */

#ifndef VTPARSE_TABLE_H_
#define VTPARSE_TABLE_H_

#include <stdint.h>
#include <drivers/vt.h>

enum vtparse_state {
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

#define VTPARSE_STATES_TOTAL 14

typedef enum vtparse_state vtparse_state_t;

#define __VTPARSE_STATE_TABLE_INDEX(state) \
		(state - 1)

#define __VTPARSE_STATE_TRANSITION_ENTRY_ENCODE(action, state) \
		((action) | ((state) << 4))

#define __VTPARSE_STATE_ACTIONS_ENTRY_ENCODE(enter, leave) \
		((enter) << 4 | (leave))

typedef uint8_t __vtparse_state_transition_entry_t;
typedef uint8_t __vtparse_state_actions_entry_t;

static inline __vtparse_state_transition_entry_t
		__vtparse_state_transition_entry(vtparse_state_t state, char ch) {
	extern const __vtparse_state_transition_entry_t
			__vtparse_state_transition_table[VTPARSE_STATES_TOTAL][0x100];
	return __vtparse_state_transition_table
			[__VTPARSE_STATE_TABLE_INDEX(state)][ch];
}

static inline __vtparse_state_actions_entry_t __vtparse_state_actions_entry(
		vtparse_state_t state) {
	extern const __vtparse_state_actions_entry_t
			__vtparse_state_actions_table[VTPARSE_STATES_TOTAL];
	return __vtparse_state_actions_table[__VTPARSE_STATE_TABLE_INDEX(state)];
}

static inline vtparse_state_t vtparse_state_transition(
		vtparse_state_t state, char ch) {
	return __vtparse_state_transition_entry(state, ch) >> 4;
}

static inline vtparse_state_t vtparse_state_action(
		vtparse_state_t state, char ch) {
	return __vtparse_state_transition_entry(state, ch) & 0xf;
}

static inline vt_action_t vtparse_state_action_enter(vtparse_state_t state) {
	return __vtparse_state_actions_entry(state) >> 4;
}

static inline vt_action_t vtparse_state_action_leave(vtparse_state_t state) {
	return __vtparse_state_actions_entry(state) & 0xf;
}

#endif /* VTPARSE_TABLE_H_ */
