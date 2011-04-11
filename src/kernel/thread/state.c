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
	[STATE_INDEX(RUNNING)] = {
		[ACTION(SUSPEND)] = STATE(SUSPENDED),
		[ACTION(SLEEP)]   = STATE(SLEEPING),
	},
	[STATE_INDEX(SLEEPING)] = {
		[ACTION(SUSPEND)] = STATE(SLEEPING_SUSPENDED),
		[ACTION(WAKE)]    = STATE(RUNNING),
	},
	[STATE_INDEX(SUSPENDED)] = {
		[ACTION(SUSPEND)] = STATE(SUSPENDED),
		[ACTION(RESUME)]  = STATE(RUNNING),
	},
	[STATE_INDEX(SLEEPING_SUSPENDED)] = {
		[ACTION(SUSPEND)] = STATE(SLEEPING_SUSPENDED),
		[ACTION(RESUME)]  = STATE(SLEEPING),
		[ACTION(WAKE)]    = STATE(SUSPENDED),
	},
};
