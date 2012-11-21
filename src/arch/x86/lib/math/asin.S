/**
 * @file
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

	.text

	.global asinf,asin,asinl

asinf:
	flds 4(%esp)
	jmp __asin

asinl:
	fldt 4(%esp)
	jmp __asin

asin:
	fldl 4(%esp)

__asin:
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fpatan
	ret
