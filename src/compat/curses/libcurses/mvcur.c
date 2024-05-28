/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include "curses_priv.h"

int mvcur(int oldrow, int oldcol, int newrow, int newcol) {
	if (newrow < oldrow) {
		_curs_putp(TPARM_1(TINFO_PARM_UP_CURSOR, oldrow - newrow));
	}
	else if (newrow > oldrow) {
		_curs_putp(TPARM_1(TINFO_PARM_DOWN_CURSOR, newrow - oldrow));
	}

	if (newcol < oldcol) {
		_curs_putp(TPARM_1(TINFO_PARM_LEFT_CURSOR, oldcol - newcol));
	}
	else if (newcol > oldcol) {
		_curs_putp(TPARM_1(TINFO_PARM_RIGHT_CURSOR, newcol - oldcol));
	}

	return OK;
}
