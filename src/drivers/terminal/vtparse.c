/*
 * VTParse - an implementation of Paul Williams' DEC compatible state machine parser
 *
 * Author: Joshua Haberman <joshua@reverberate.org>
 *
 * This code is in the public domain.
 */

#include "vtparse.h"

#include "vt.h"
#include "types.h"
#include "common.h"

#define ACTION(state_change) (state_change & 0x0F)
#define STATE(state_change)  (state_change >> 4)

void vtparse_init(VTPARSER *parser, VTPARSE_CALLBACK cb, char(*getc)(
		VTPARSER *parser)) {
	parser->state = VTPARSE_STATE_GROUND;
	parser->intermediate_chars[0] = '\0';
	parser->params->length = 0;
	parser->ignore_flagged = 0;
	parser->cb = cb;
	parser->getc = getc;
}

static void do_action(VTPARSER *parser, VT_ACTION action, char ch) {
	/* Some actions we handle internally (like parsing parameters), others
	 * we hand to our client for processing */

	switch (action) {
	case VT_ACTION_PRINT:
	case VT_ACTION_EXECUTE:
	case VT_ACTION_HOOK:
	case VT_ACTION_PUT:
	case VT_ACTION_OSC_START:
	case VT_ACTION_OSC_PUT:
	case VT_ACTION_OSC_END:
	case VT_ACTION_UNHOOK:
	case VT_ACTION_CSI_DISPATCH:
	case VT_ACTION_ESC_DISPATCH:
		parser->cb(parser, action, ch);
		break;

	case VT_ACTION_IGNORE:
		/* do nothing */
		break;

	case VT_ACTION_COLLECT: {
		/* Append the character to the intermediate params */
		int num_intermediate_chars = strlen((char*) parser->intermediate_chars);

		if (num_intermediate_chars + 1 > MAX_INTERMEDIATE_CHARS)
			parser->ignore_flagged = 1;
		else
			parser->intermediate_chars[num_intermediate_chars++] = ch;

		break;
	}

	case VT_ACTION_PARAM: {
		/* process the param character */
		if (ch == ';') {
			parser->params->length += 1;
			parser->params->data[parser->params->length - 1] = 0;
		} else {
			/* the character is a digit */
			int current_param;

			if (parser->params->length == 0) {
				parser->params->length = 1;
				parser->params->data[0] = 0;
			}

			current_param = parser->params->length - 1;
			parser->params->data[current_param] *= 10;
			parser->params->data[current_param] += (ch - '0');
		}

		break;
	}

	case VT_ACTION_CLEAR:
		parser->intermediate_chars[0] = '\0';
		parser->params->length = 0;
		parser->ignore_flagged = 0;
		break;

	default:
		TRACE("Internal error, unknown action %d", action);
	}
}

static void do_state_change(VTPARSER *parser, state_change_t change, char ch) {
	/* A state change is an action and/or a new state to transition to. */

	VTPARSE_STATE new_state = STATE(change);
	VT_ACTION action = ACTION(change);

	if (new_state) {
		/* Perform up to three actions:
		 *   1. the exit action of the old state
		 *   2. the action associated with the transition
		 *   3. the entry actionk of the new action
		 */

		VT_ACTION exit_action = EXIT_ACTIONS[parser->state];
		VT_ACTION entry_action = ENTRY_ACTIONS[new_state];

		if (exit_action)
			do_action(parser, exit_action, 0);

		if (action)
			do_action(parser, action, ch);

		if (entry_action)
			do_action(parser, entry_action, 0);

		parser->state = new_state;
	} else {
		do_action(parser, action, ch);
	}
}

void vtparse(VTPARSER *parser) {
	char ch;
	state_change_t change;
	while ((parser->getc != NULL) && (ch = parser->getc(parser))) {

		/* If a transition is defined from the "anywhere" state, always
		 * use that.  Otherwise use the transition from the current state. */

		change = STATE_TABLE[VTPARSE_STATE_ANYWHERE][ch];

		if (!change)
			change = STATE_TABLE[parser->state][ch];

		do_state_change(parser, change, ch);
	}
}

