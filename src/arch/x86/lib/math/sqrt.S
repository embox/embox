/**
 * @file
 *
 * @date 09.11.2012
 * @author Anton Bulychev
 */

	.text

	.global sqrtf,sqrt,sqrtl

sqrtf:
	flds 4(%esp)
	fsqrt
	ret

sqrt:
	fldl 4(%esp)
	fsqrt
	ret

sqrtl:
	fldt 4(%esp)
	fsqrt
	ret
