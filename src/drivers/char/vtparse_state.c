/**
 * @file
 * @brief State machine matrix for VTParse - DEC compatible parser.
 * @details VTParse is an implementation of Paul Williams' DEC compatible state
 * machine parser. For further information please refer
 * to http://vt100.net/emu/dec_ansi_parser.
 *
 * @date 04.02.2009
 * @author Joshua Haberman <joshua@reverberate.org>
 *          - Initial implementation using code generation from Ruby script
 * @author Eldar Abusalimov
 *          - Rewriting by hand using GNU C language extensions
 */

#include <drivers/vtparse_state.h>

/* Perform both action and the state change. */
#define ACTION_TRANSIT(action, state) \
	__VTPARSE_STATE_TRANSITION_ENTRY_ENCODE( \
			VT_ACTION_ ## action, VTPARSE_STATE_ ## state)

/* Perform action without doing the state change. */
#define ACTION_STAY(action) \
	/* Parser does not perform a transition if state change field is 0.*/ \
	__VTPARSE_STATE_TRANSITION_ENTRY_ENCODE( \
			VT_ACTION_ ## action, 0)

#define ACTION(enter, leave) \
	__VTPARSE_STATE_ACTIONS_ENTRY_ENCODE( \
			VT_ACTION_ ## enter, VT_ACTION_ ## leave)

#define STATE_INDEX(state) \
	__VTPARSE_STATE_TABLE_INDEX(VTPARSE_STATE_ ## state)

const __vtparse_state_actions_entry_t  __vtparse_state_actions_table
				[VTPARSE_STATES_TOTAL] = {
	[STATE_INDEX(ESCAPE)]          = ACTION(CLEAR, NONE),
	[STATE_INDEX(CSI_ENTRY)]       = ACTION(CLEAR, NONE),
	[STATE_INDEX(DCS_ENTRY)]       = ACTION(CLEAR, NONE),
	[STATE_INDEX(DCS_PASSTHROUGH)] = ACTION(HOOK, UNHOOK),
	[STATE_INDEX(OSC_STRING)]      = ACTION(OSC_START, OSC_END),
};

/* Every state includes some common transitions. */
#define ANYWHERE_TRANSITIONS \
		[0x18         ] = ACTION_TRANSIT(EXECUTE, GROUND),         \
		[0x1a         ] = ACTION_TRANSIT(EXECUTE, GROUND),         \
		[0x80 ... 0x8f] = ACTION_TRANSIT(EXECUTE, GROUND),         \
		[0x91 ... 0x97] = ACTION_TRANSIT(EXECUTE, GROUND),         \
		[0x99         ] = ACTION_TRANSIT(EXECUTE, GROUND),         \
		[0x9a         ] = ACTION_TRANSIT(EXECUTE, GROUND),         \
		[0x9c         ] = ACTION_TRANSIT(NONE, GROUND), /* TODO check it. */ \
		[0x1b         ] = ACTION_TRANSIT(NONE, ESCAPE),            \
		[0x98         ] = ACTION_TRANSIT(NONE, SOS_PM_APC_STRING), \
		[0x9e         ] = ACTION_TRANSIT(NONE, SOS_PM_APC_STRING), \
		[0x9f         ] = ACTION_TRANSIT(NONE, SOS_PM_APC_STRING), \
		[0x90         ] = ACTION_TRANSIT(NONE, DCS_ENTRY),         \
		[0x9d         ] = ACTION_TRANSIT(NONE, OSC_STRING),        \
		[0x9b         ] = ACTION_TRANSIT(NONE, CSI_ENTRY),

__extension__ /* GNU C designated initializers. */
const __vtparse_state_transition_entry_t  __vtparse_state_transition_table
				[VTPARSE_STATES_TOTAL][0x100] = {
	[STATE_INDEX(CSI_ENTRY)] = {
		[0x00 ... 0x17] = ACTION_STAY(EXECUTE),
		[0x19         ] = ACTION_STAY(EXECUTE),
		[0x1c ... 0x1f] = ACTION_STAY(EXECUTE),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x20 ... 0x2f] = ACTION_TRANSIT(COLLECT, CSI_INTERMEDIATE),
		[0x3a         ] = ACTION_TRANSIT(NONE, CSI_IGNORE),
		[0x30 ... 0x39] = ACTION_TRANSIT(PARAM, CSI_PARAM),
		[0x3b         ] = ACTION_TRANSIT(PARAM, CSI_PARAM),
		[0x3c ... 0x3f] = ACTION_TRANSIT(COLLECT, CSI_PARAM),
		[0x40 ... 0x7e] = ACTION_TRANSIT(CSI_DISPATCH,GROUND),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(CSI_IGNORE)] = {
		[0x00 ... 0x17] = ACTION_STAY(EXECUTE),
		[0x19         ] = ACTION_STAY(EXECUTE),
		[0x1c ... 0x1f] = ACTION_STAY(EXECUTE),
		[0x20 ... 0x3f] = ACTION_STAY(IGNORE),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x40 ... 0x7e] = ACTION_TRANSIT(NONE, GROUND),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(CSI_INTERMEDIATE)] = {
		[0x00 ... 0x17] = ACTION_STAY(EXECUTE),
		[0x19         ] = ACTION_STAY(EXECUTE),
		[0x1c ... 0x1f] = ACTION_STAY(EXECUTE),
		[0x20 ... 0x2f] = ACTION_STAY(COLLECT),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x30 ... 0x3f] = ACTION_TRANSIT(NONE, CSI_IGNORE),
		[0x40 ... 0x7e] = ACTION_TRANSIT(CSI_DISPATCH, GROUND),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(CSI_PARAM)] = {
		[0x00 ... 0x17] = ACTION_STAY(EXECUTE),
		[0x19         ] = ACTION_STAY(EXECUTE),
		[0x1c ... 0x1f] = ACTION_STAY(EXECUTE),
		[0x30 ... 0x39] = ACTION_STAY(PARAM),
		[0x3b         ] = ACTION_STAY(PARAM),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x3a         ] = ACTION_TRANSIT(NONE, CSI_IGNORE),
		[0x3c ... 0x3f] = ACTION_TRANSIT(NONE, CSI_IGNORE),
		[0x20 ... 0x2f] = ACTION_TRANSIT(COLLECT, CSI_INTERMEDIATE),
		[0x40 ... 0x7e] = ACTION_TRANSIT(CSI_DISPATCH, GROUND),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(DCS_ENTRY)] = {
		[0x00 ... 0x17] = ACTION_STAY(IGNORE),
		[0x19         ] = ACTION_STAY(IGNORE),
		[0x1c ... 0x1f] = ACTION_STAY(IGNORE),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x3a         ] = ACTION_TRANSIT(NONE, DCS_IGNORE),
		[0x20 ... 0x2f] = ACTION_TRANSIT(COLLECT, DCS_INTERMEDIATE),
		[0x30 ... 0x39] = ACTION_TRANSIT(PARAM, DCS_PARAM),
		[0x3b         ] = ACTION_TRANSIT(PARAM, DCS_PARAM),
		[0x3c ... 0x3f] = ACTION_TRANSIT(COLLECT, DCS_PARAM),
		[0x40 ... 0x7e] = ACTION_TRANSIT(NONE, DCS_PASSTHROUGH),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(DCS_IGNORE)] = {
		[0x00 ... 0x17] = ACTION_STAY(IGNORE),
		[0x19         ] = ACTION_STAY(IGNORE),
		[0x1c ... 0x1f] = ACTION_STAY(IGNORE),
		[0x20 ... 0x7f] = ACTION_STAY(IGNORE),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(DCS_INTERMEDIATE)] = {
		[0x00 ... 0x17] = ACTION_STAY(IGNORE),
		[0x19         ] = ACTION_STAY(IGNORE),
		[0x1c ... 0x1f] = ACTION_STAY(IGNORE),
		[0x20 ... 0x2f] = ACTION_STAY(COLLECT),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x30 ... 0x3f] = ACTION_TRANSIT(NONE, DCS_IGNORE),
		[0x40 ... 0x7e] = ACTION_TRANSIT(NONE, DCS_PASSTHROUGH),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(DCS_PARAM)] = {
		[0x00 ... 0x17] = ACTION_STAY(IGNORE),
		[0x19         ] = ACTION_STAY(IGNORE),
		[0x1c ... 0x1f] = ACTION_STAY(IGNORE),
		[0x30 ... 0x39] = ACTION_STAY(PARAM),
		[0x3b         ] = ACTION_STAY(PARAM),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x3a         ] = ACTION_TRANSIT(NONE, DCS_IGNORE),
		[0x3c ... 0x3f] = ACTION_TRANSIT(NONE, DCS_IGNORE),
		[0x20 ... 0x2f] = ACTION_TRANSIT(COLLECT, DCS_INTERMEDIATE),
		[0x40 ... 0x7e] = ACTION_TRANSIT(NONE, DCS_PASSTHROUGH),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(DCS_PASSTHROUGH)] = {
		[0x00 ... 0x17] = ACTION_STAY(PUT),
		[0x19         ] = ACTION_STAY(PUT),
		[0x1c ... 0x1f] = ACTION_STAY(PUT),
		[0x20 ... 0x7e] = ACTION_STAY(PUT),
		[0x7f         ] = ACTION_STAY(IGNORE),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(ESCAPE)] = {
		[0x00 ... 0x17] = ACTION_STAY(EXECUTE),
		[0x19         ] = ACTION_STAY(EXECUTE),
		[0x1c ... 0x1f] = ACTION_STAY(EXECUTE),
#if 0
		[0x7f         ] = ACTION_STAY(IGNORE),
#else
		[0x7f         ] = ACTION_STAY(EXECUTE),
#endif
		[0x20 ... 0x2f] = ACTION_TRANSIT(COLLECT, ESCAPE_INTERMEDIATE),
		[0x30 ... 0x4f] = ACTION_TRANSIT(ESC_DISPATCH, GROUND),
		[0x51 ... 0x57] = ACTION_TRANSIT(ESC_DISPATCH, GROUND),
		[0x59         ] = ACTION_TRANSIT(ESC_DISPATCH, GROUND),
		[0x5a         ] = ACTION_TRANSIT(ESC_DISPATCH, GROUND),
		[0x5c         ] = ACTION_TRANSIT(ESC_DISPATCH, GROUND),
		[0x60 ... 0x7e] = ACTION_TRANSIT(ESC_DISPATCH, GROUND),
		[0x5b         ] = ACTION_TRANSIT(NONE, CSI_ENTRY),
		[0x5d         ] = ACTION_TRANSIT(NONE, OSC_STRING),
		[0x50         ] = ACTION_TRANSIT(NONE, DCS_ENTRY),
		[0x58         ] = ACTION_TRANSIT(NONE, SOS_PM_APC_STRING),
		[0x5e         ] = ACTION_TRANSIT(NONE, SOS_PM_APC_STRING),
		[0x5f         ] = ACTION_TRANSIT(NONE, SOS_PM_APC_STRING),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(ESCAPE_INTERMEDIATE)] = {
		[0x00 ... 0x17] = ACTION_STAY(EXECUTE),
		[0x19         ] = ACTION_STAY(EXECUTE),
		[0x1c ... 0x1f] = ACTION_STAY(EXECUTE),
		[0x20 ... 0x2f] = ACTION_STAY(COLLECT),
		[0x7f         ] = ACTION_STAY(IGNORE),
		[0x30 ... 0x7e] = ACTION_TRANSIT(ESC_DISPATCH, GROUND),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(GROUND)] = {
		[0x00 ... 0x17] = ACTION_STAY(EXECUTE),
		[0x19         ] = ACTION_STAY(EXECUTE),
		[0x1c ... 0x1f] = ACTION_STAY(EXECUTE),
		[0x20 ... 0x7e] = ACTION_STAY(PRINT),
#if 0
		[0x7f         ] = ACTION_STAY(PRINT),
#else
		[0x7f         ] = ACTION_STAY(EXECUTE),
#endif
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(OSC_STRING)] = {
		[0x00 ... 0x17] = ACTION_STAY(IGNORE),
		[0x19         ] = ACTION_STAY(IGNORE),
		[0x1c ... 0x1f] = ACTION_STAY(IGNORE),
		[0x20 ... 0x7f] = ACTION_STAY(OSC_PUT),
		ANYWHERE_TRANSITIONS
	},
	[STATE_INDEX(SOS_PM_APC_STRING)] = {
		[0x00 ... 0x17] = ACTION_STAY(IGNORE),
		[0x19         ] = ACTION_STAY(IGNORE),
		[0x1c ... 0x1f] = ACTION_STAY(IGNORE),
		[0x20 ... 0x7f] = ACTION_STAY(IGNORE),
		ANYWHERE_TRANSITIONS
	},
};

