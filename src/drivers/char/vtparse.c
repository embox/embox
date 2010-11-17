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

#include <embox/kernel.h>

#include <drivers/vtparse.h>
#include <drivers/vtparse_table.h>

struct vtparse *vtparse_init(struct vtparse *parser, vtparse_callback_t cb) {
	if (parser == NULL) {
		return NULL;
	}

	parser->state = VTPARSE_STATE_GROUND;
	parser->token->params = parser->params;
	parser->token->params_len = 0;
	parser->token->attrs_len = 0;
	parser->cb = cb;

	return parser;
}

static void do_action(struct vtparse *parser, VT_ACTION action, char ch) {
	/* Some actions we handle internally (like parsing parameters), others
	 * we hand to our client for processing */
	VT_TOKEN *token = parser->token;
	token->action = action;
	token->ch = ch;

	switch (action) {
	case VT_ACTION_PRINT:
	case VT_ACTION_EXECUTE:
	case VT_ACTION_CSI_DISPATCH:
	case VT_ACTION_ESC_DISPATCH:
		parser->cb(parser, parser->token);
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
			parser->params[token->params_len++] = 0;
		} else {
			/* the character is a digit */
			int *current_param;

			if (token->params_len == 0) {
				token->params_len = 1;
				parser->params[0] = 0;
			}

			current_param = &parser->params[token->params_len - 1];
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

static void do_state_change(struct vtparse *parser,
		vtparse_state_change_t change, char ch) {
	/* A state change is an action and/or a new state to transition to. */
	enum vtparse_state new_state = VTPARSE_TRANSITION_STATE(change);
	VT_ACTION action = VTPARSE_TRANSITION_ACTION(change);

	if (new_state) {
		/* Perform up to three actions:
		 *   1. the exit action of the old state
		 *   2. the action associated with the transition
		 *   3. the entry action of the new state
		 */

		VT_ACTION exit_action = vtparse_state_exit_actions[parser->state];
		VT_ACTION entry_action = vtparse_state_entry_actions[new_state];

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

void vtparse(struct vtparse *parser, unsigned char ch) {
	do_state_change(parser, vtparse_state_table[parser->state][ch], ch);
}

