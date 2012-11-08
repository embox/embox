/**
 * @file
 *
 * @date 08.11.2012
 * @author Anton Bulychev
 */

	.text

	.global sinf,sin,sinl

sinf:
	flds	4(%esp)
	jmp	1f

sin:
	fldl	4(%esp)
1:
	fsin
	fnstsw	%ax
	testb	$0x04, %ah
	je	3f
	fldpi
	fadd	%st
	fxch	%st(1)
2:
	fprem1
	fnstsw	%ax
	testb	$0x04, %ah
	jne	2b
	fstp	%st(1)
	fsin
3:
	ret

sinl:
	fldt	4(%esp)
	jmp	1b
