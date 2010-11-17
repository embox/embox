/**
 * @file
 *
 * @date 04.02.2009
 * @author Eldar Abusalimov
 */

#include <drivers/vtparse_table.h>

/* Perform both action and the state change. */
#define TRANSIT(vt_action, vtparse_state) \
	((state_change_t) (__ACTION(vt_action) | __STATE(vtparse_state)))

/* Perform action without doing the state change. */
#define ACTION(vt_action) \
	((state_change_t) __ACTION(vt_action))

#define __ACTION(vt_action) \
	(VT_ACTION_ ## vt_action)
#define __STATE(vtparse_state) \
	((VTPARSE_STATE_ ## vtparse_state) << 4)

__extension__ const state_change_t STATE_TABLE[15][256] = {
	[VTPARSE_STATE_ANYWHERE] = {
		[0x18         ] = TRANSIT(EXECUTE, GROUND),
		[0x1a         ] = TRANSIT(EXECUTE, GROUND),
		[0x80 ... 0x8f] = TRANSIT(EXECUTE, GROUND),
		[0x91 ... 0x97] = TRANSIT(EXECUTE, GROUND),
		[0x99         ] = TRANSIT(EXECUTE, GROUND),
		[0x9a         ] = TRANSIT(EXECUTE, GROUND),
		[0x9c         ] = TRANSIT(EXECUTE, GROUND),
		[0x1b         ] = TRANSIT(NONE, ESCAPE),
		[0x98         ] = TRANSIT(NONE, SOS_PM_APC_STRING),
		[0x9e         ] = TRANSIT(NONE, SOS_PM_APC_STRING),
		[0x9f         ] = TRANSIT(NONE, SOS_PM_APC_STRING),
		[0x90         ] = TRANSIT(NONE, DCS_ENTRY),
		[0x9d         ] = TRANSIT(NONE, OSC_STRING),
		[0x9b         ] = TRANSIT(NONE, CSI_ENTRY),
	},
	[VTPARSE_STATE_CSI_ENTRY] = {
		[0x00 ... 0x17] = ACTION(EXECUTE),
		[0x19         ] = ACTION(EXECUTE),
		[0x1c ... 0x1f] = ACTION(EXECUTE),
		[0x7f         ] = ACTION(IGNORE),
		[0x20 ... 0x2f] = TRANSIT(COLLECT, CSI_INTERMEDIATE),
		[0x3a         ] = TRANSIT(NONE, CSI_IGNORE),
		[0x30 ... 0x39] = TRANSIT(PARAM, CSI_PARAM),
		[0x3b         ] = TRANSIT(PARAM, CSI_PARAM),
		[0x3c ... 0x3f] = TRANSIT(COLLECT, CSI_PARAM),
		[0x40 ... 0x7e] = TRANSIT(CSI_DISPATCH,GROUND),
	},
	[VTPARSE_STATE_CSI_IGNORE] = {
		[0x00 ... 0x17] = ACTION(EXECUTE),
		[0x19         ] = ACTION(EXECUTE),
		[0x1c ... 0x1f] = ACTION(EXECUTE),
		[0x20 ... 0x3f] = ACTION(IGNORE),
		[0x7f         ] = ACTION(IGNORE),
		[0x40 ... 0x7e] = TRANSIT(NONE, GROUND),
	},
	[VTPARSE_STATE_CSI_INTERMEDIATE] = {
		[0x00 ... 0x17] = ACTION(EXECUTE),
		[0x19         ] = ACTION(EXECUTE),
		[0x1c ... 0x1f] = ACTION(EXECUTE),
		[0x20 ... 0x2f] = ACTION(COLLECT),
		[0x7f         ] = ACTION(IGNORE),
		[0x30 ... 0x3f] = TRANSIT(NONE, CSI_IGNORE),
		[0x40 ... 0x7e] = TRANSIT(CSI_DISPATCH, GROUND),
	},
	[VTPARSE_STATE_CSI_PARAM] = {
		[0x00 ... 0x17] = ACTION(EXECUTE),
		[0x19         ] = ACTION(EXECUTE),
		[0x1c ... 0x1f] = ACTION(EXECUTE),
		[0x30 ... 0x39] = ACTION(PARAM),
		[0x3b         ] = ACTION(PARAM),
		[0x7f         ] = ACTION(IGNORE),
		[0x3a         ] = TRANSIT(NONE, CSI_IGNORE),
		[0x3c ... 0x3f] = TRANSIT(NONE, CSI_IGNORE),
		[0x20 ... 0x2f] = TRANSIT(COLLECT, CSI_INTERMEDIATE),
		[0x40 ... 0x7e] = TRANSIT(CSI_DISPATCH, GROUND),
	},
	[VTPARSE_STATE_DCS_ENTRY] = {
		[0x00 ... 0x17] = ACTION(IGNORE),
		[0x19         ] = ACTION(IGNORE),
		[0x1c ... 0x1f] = ACTION(IGNORE),
		[0x7f         ] = ACTION(IGNORE),
		[0x3a         ] = TRANSIT(NONE, DCS_IGNORE),
		[0x20 ... 0x2f] = TRANSIT(COLLECT, DCS_INTERMEDIATE),
		[0x30 ... 0x39] = TRANSIT(PARAM, DCS_PARAM),
		[0x3b         ] = TRANSIT(PARAM, DCS_PARAM),
		[0x3c ... 0x3f] = TRANSIT(COLLECT, DCS_PARAM),
		[0x40 ... 0x7e] = TRANSIT(NONE, DCS_PASSTHROUGH),
	},
	[VTPARSE_STATE_DCS_IGNORE] = {
		[0x00 ... 0x17] = ACTION(IGNORE),
		[0x19         ] = ACTION(IGNORE),
		[0x1c ... 0x1f] = ACTION(IGNORE),
		[0x20 ... 0x7f] = ACTION(IGNORE),
		[0x9c         ] = TRANSIT(NONE, GROUND),
	},
	[VTPARSE_STATE_DCS_INTERMEDIATE] = {
		[0x00 ... 0x17] = ACTION(IGNORE),
		[0x19         ] = ACTION(IGNORE),
		[0x1c ... 0x1f] = ACTION(IGNORE),
		[0x20 ... 0x2f] = ACTION(COLLECT),
		[0x7f         ] = ACTION(IGNORE),
		[0x30 ... 0x3f] = TRANSIT(NONE, DCS_IGNORE),
		[0x40 ... 0x7e] = TRANSIT(NONE, DCS_PASSTHROUGH),
	},
	[VTPARSE_STATE_DCS_PARAM] = {
		[0x00 ... 0x17] = ACTION(IGNORE),
		[0x19         ] = ACTION(IGNORE),
		[0x1c ... 0x1f] = ACTION(IGNORE),
		[0x30 ... 0x39] = ACTION(PARAM),
		[0x3b         ] = ACTION(PARAM),
		[0x7f         ] = ACTION(IGNORE),
		[0x3a         ] = TRANSIT(NONE, DCS_IGNORE),
		[0x3c ... 0x3f] = TRANSIT(NONE, DCS_IGNORE),
		[0x20 ... 0x2f] = TRANSIT(COLLECT, DCS_INTERMEDIATE),
		[0x40 ... 0x7e] = TRANSIT(NONE, DCS_PASSTHROUGH),
	},
	[VTPARSE_STATE_DCS_PASSTHROUGH] = {
		[0x00 ... 0x17] = ACTION(PUT),
		[0x19         ] = ACTION(PUT),
		[0x1c ... 0x1f] = ACTION(PUT),
		[0x20 ... 0x7e] = ACTION(PUT),
		[0x7f         ] = ACTION(IGNORE),
		[0x9c         ] = TRANSIT(NONE, GROUND),
	},
	[VTPARSE_STATE_ESCAPE] = {
		[0x00 ... 0x17] = ACTION(EXECUTE),
		[0x19         ] = ACTION(EXECUTE),
		[0x1c ... 0x1f] = ACTION(EXECUTE),
#if 0
		[0x7f         ] = ACTION(IGNORE),
#else
		[0x7f         ] = ACTION(EXECUTE),
#endif
		[0x20 ... 0x2f] = TRANSIT(COLLECT, ESCAPE_INTERMEDIATE),
		[0x30 ... 0x4f] = TRANSIT(ESC_DISPATCH, GROUND),
		[0x51 ... 0x57] = TRANSIT(ESC_DISPATCH, GROUND),
		[0x59         ] = TRANSIT(ESC_DISPATCH, GROUND),
		[0x5a         ] = TRANSIT(ESC_DISPATCH, GROUND),
		[0x5c         ] = TRANSIT(ESC_DISPATCH, GROUND),
		[0x60 ... 0x7e] = TRANSIT(ESC_DISPATCH, GROUND),
		[0x5b         ] = TRANSIT(NONE, CSI_ENTRY),
		[0x5d         ] = TRANSIT(NONE, OSC_STRING),
		[0x50         ] = TRANSIT(NONE, DCS_ENTRY),
		[0x58         ] = TRANSIT(NONE, SOS_PM_APC_STRING),
		[0x5e         ] = TRANSIT(NONE, SOS_PM_APC_STRING),
		[0x5f         ] = TRANSIT(NONE, SOS_PM_APC_STRING),
	},
	[VTPARSE_STATE_ESCAPE_INTERMEDIATE] = {
		[0x00 ... 0x17] = ACTION(EXECUTE),
		[0x19         ] = ACTION(EXECUTE),
		[0x1c ... 0x1f] = ACTION(EXECUTE),
		[0x20 ... 0x2f] = ACTION(COLLECT),
		[0x7f         ] = ACTION(IGNORE),
		[0x30 ... 0x7e] = TRANSIT(ESC_DISPATCH, GROUND),
	},
	[VTPARSE_STATE_GROUND] = {
		[0x00 ... 0x17] = ACTION(EXECUTE),
		[0x19         ] = ACTION(EXECUTE),
		[0x1c ... 0x1f] = ACTION(EXECUTE),
		[0x20 ... 0x7e] = ACTION(PRINT),
#if 0
		[0x7f         ] = ACTION(PRINT),
#else
		[0x7f         ] = ACTION(EXECUTE),
#endif
		[0x80 ... 0x8f] = ACTION(EXECUTE),
		[0x91 ... 0x9a] = ACTION(EXECUTE),
		[0x9c         ] = ACTION(EXECUTE),
	},
	[VTPARSE_STATE_OSC_STRING] = {
		[0x00 ... 0x17] = ACTION(IGNORE),
		[0x19         ] = ACTION(IGNORE),
		[0x1c ... 0x1f] = ACTION(IGNORE),
		[0x20 ... 0x7f] = TRANSIT(OSC_PUT, ANYWHERE),
		[0x9c         ] = TRANSIT(NONE, GROUND),
	},
	[VTPARSE_STATE_SOS_PM_APC_STRING] = {
		[0x00 ... 0x17] = ACTION(IGNORE),
		[0x19         ] = ACTION(IGNORE),
		[0x1c ... 0x1f] = ACTION(IGNORE),
		[0x20 ... 0x7f] = ACTION(IGNORE),
		[0x9c         ] = TRANSIT(NONE, GROUND),
	},
};

const VT_ACTION ENTRY_ACTIONS[] = {
	[VTPARSE_STATE_CSI_ENTRY]       = VT_ACTION_CLEAR,
	[VTPARSE_STATE_DCS_ENTRY]       = VT_ACTION_CLEAR,
	[VTPARSE_STATE_DCS_PASSTHROUGH] = VT_ACTION_HOOK,
	[VTPARSE_STATE_ESCAPE]          = VT_ACTION_CLEAR,
	[VTPARSE_STATE_OSC_STRING]      = VT_ACTION_OSC_START,
};

const VT_ACTION EXIT_ACTIONS[] = {
	[VTPARSE_STATE_DCS_PASSTHROUGH] = VT_ACTION_UNHOOK,
	[VTPARSE_STATE_OSC_STRING]      = VT_ACTION_OSC_END,
};

