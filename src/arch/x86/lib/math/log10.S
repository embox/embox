/**
 * @file
 *
 * @date 09.11.2012
 * @author Anton Bulychev
 */

	.text

	.global log10f,log10,log10l

log10f:
	fldlg2
	flds 4(%esp)
	fyl2x
	ret

log10l:
	fldlg2
	fldt 4(%esp)
	fyl2x
	ret

log10:
	fldlg2
	fldl 4(%esp)
	fyl2x
	ret
