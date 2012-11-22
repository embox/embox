/**
 * @file
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

	.text

	.global acosf,acos,acosl

acosf:
	flds 4(%esp)
	jmp __acos

acosl:
	fldt 4(%esp)
	jmp __acos

acos:
	fldl 4(%esp)

__acos:
	fld	%st
	fmul	%st
	fld1
	fsubp
	fsqrt
	fxch	%st(1)
	fpatan
	ret
