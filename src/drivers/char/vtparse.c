/**
 * @file
 * @brief VTParse - an implementation of
 * Paul Williams' DEC compatible state machine parser
 * This code is in the public domain.
 *
 * @date 04.02.2009
 * @author Joshua Haberman <joshua@reverberate.org>
 * @author Eldar Abusalimov
 *         - adaptation for using with terminal driver.
 *           As we use only Plain, ESC and CSI-based tokens all the others are ignored
 *           (in spite of the fact that the Parser is capable to handle them correctly).
 */
#include <drivers/vtparse.h>
#include <embox/kernel.h>

#define ACTION(state_change) (state_change & 0x0F)
#define STATE(state_change)  (state_change >> 4)

VTPARSER * vtparse_init(VTPARSER *this, VTPARSE_CALLBACK cb) {
	if (this == NULL) {
		return NULL;
	}

	this->state = VTPARSE_STATE_GROUND;
	this->token->params = this->params;
	this->token->params_len = 0;
	this->token->attrs_len = 0;
	this->cb = cb;

	return this;
}

static void do_action(VTPARSER *this, VT_ACTION action, char ch) {
	/* Some actions we handle internally (like parsing parameters), others
	 * we hand to our client for processing */
	VT_TOKEN *token = this->token;
	token->action = action;
	token->ch = ch;

	switch (action) {
	case VT_ACTION_PRINT:
	case VT_ACTION_EXECUTE:
	case VT_ACTION_CSI_DISPATCH:
	case VT_ACTION_ESC_DISPATCH:
		this->cb(this, this->token);
		break;

	case VT_ACTION_COLLECT: {
		/* Append the character to the intermediate params */

		if (token->attrs_len < VT_TOKEN_MAX_CHARS + 1) {
			token->attrs[token->attrs_len++] = ch;
		}

		break;
	}

	case VT_ACTION_PARAM: {
		/* process the param character */
		if (ch == ';') {
			this->params[token->params_len++] = 0;
		} else {
			/* the character is a digit */
			int *current_param;

			if (token->params_len == 0) {
				token->params_len = 1;
				this->params[0] = 0;
			}

			current_param = &this->params[token->params_len - 1];
			*current_param *= 10;
			*current_param += (ch - '0');
		}

		break;
	}

	case VT_ACTION_CLEAR:
		token->params_len = 0;
		token->attrs_len = 0;
		break;

	case VT_ACTION_OSC_START:
	case VT_ACTION_OSC_PUT:
	case VT_ACTION_OSC_END:
		/* Operating System Command */
		/* ignore them as unused in our system
		  -- Eldar*/
	case VT_ACTION_HOOK:
	case VT_ACTION_PUT:
	case VT_ACTION_UNHOOK:
		/* device control strings */
		/* ignore them as unused in our system
		  -- Eldar*/
	case VT_ACTION_IGNORE:
		/* do nothing */
		break;

	default:
		LOG_ERROR("vtparse: unknown action %d\n", action);
	}
}

static void do_state_change(VTPARSER *this, state_change_t change, char ch) {
	/* A state change is an action and/or a new state to transition to. */
	VTPARSE_STATE new_state = STATE(change);
	VT_ACTION action = ACTION(change);

	if (new_state) {
		/* Perform up to three actions:
		 *   1. the exit action of the old state
		 *   2. the action associated with the transition
		 *   3. the entry action of the new state
		 */

		VT_ACTION exit_action = vtparse_state_exit_actions[this->state];
		VT_ACTION entry_action = vtparse_state_entry_actions[new_state];

		if (exit_action)
			do_action(this, exit_action, 0);

		if (action)
			do_action(this, action, ch);

		if (entry_action)
			do_action(this, entry_action, 0);

		this->state = new_state;
	} else {
		do_action(this, action, ch);
	}
}

void vtparse(VTPARSER *this, unsigned char ch) {
	static state_change_t change;

	/* If a transition is defined from the "anywhere" state, always
	 * use that.  Otherwise use the transition from the current state. */

	change = vtparse_state_table[VTPARSE_STATE_ANYWHERE][ch];

	if (!change)
		change = vtparse_state_table[this->state][ch];

	do_state_change(this, change, ch);
}

