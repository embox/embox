/**
 * @file
 * @brief TODO
 *
 * @date 28.03.2011
 * @author Alina Kramar
 */

#include "state.h"

#define STATE(state) \
	THREAD_STATE_ ## state

#define ACTION(action) \
	THREAD_STATE_ACTION_ ## action

#define STATE_INDEX(state) \
	(STATE(state) - 1)

const enum thread_state
		__thread_state_transition_table[__THREAD_STATE_TOTAL ][__THREAD_STATE_ACTION_TOTAL] = {
	[STATE_INDEX(TERMINATE)] = {
		[ACTION(START)]  = STATE(RUNNING),
		[ACTION(STOP)]   = STATE(TERMINATE),
	},
	[STATE_INDEX(RUNNING)] = {
		[ACTION(STOP)]    = STATE(TERMINATE),
		[ACTION(SUSPEND)] = STATE(SUSP),
		[ACTION(SLEEP)]   = STATE(WAIT),
	},
	[STATE_INDEX(WAIT)] = {
		[ACTION(STOP)]    = STATE(TERMINATE),
		[ACTION(SUSPEND)] = STATE(WAIT_SUSP),
		[ACTION(WAKE)]    = STATE(RUNNING),
	},
	[STATE_INDEX(SUSP)] = {
		[ACTION(STOP)]    = STATE(TERMINATE),
		[ACTION(SUSPEND)] = STATE(SUSP),
		[ACTION(RESUME)]  = STATE(RUNNING),
	},
	[STATE_INDEX(WAIT_SUSP)] = {
		[ACTION(STOP)]    = STATE(TERMINATE),
		[ACTION(SUSPEND)] = STATE(WAIT_SUSP),
		[ACTION(RESUME)]  = STATE(WAIT),
		[ACTION(WAKE)]    = STATE(SUSP),
	},
};
