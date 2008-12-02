/*
 * mul_div.S
 *
 *  Created on: Dec 2, 2008
 *      Author: anton
 */
 	.global .umul, .udiv
.umul :
 	or  %o0, %o1, %o4
 	mov  %o0, %y
 	andncc  %o4, 0xfff, %o5
 	be  mul_shortway
 	andcc  %g0, %g0, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %g0, %o4
 	retl
 	rd  %y, %o0

mul_shortway :
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %o1, %o4
 	mulscc  %o4, %g0, %o4
 	rd  %y, %o5
 	sll  %o4, 0xc, %o4
 	srl  %o5, 0x14, %o5
 	retl
 	or  %o5, %o4, %o0

.udiv :
 	b  ready_to_divide
 	clr  %g3

.div :
 	orcc  %o1, %o0, %g0
 	bge  ready_to_divide
 	xor  %o1, %o0, %g3
 	tst  %o1
 	bge  .div+0x20
 	tst  %o0
 	bge  ready_to_divide
 	neg  %o1
 	neg  %o0

ready_to_divide :
 	orcc  %o1, %g0, %o5
 	bne  ready_to_divide+0x18
 	mov  %o0, %o3
 	ta  2
 	retl
 	mov  %g0, %o0
 	cmp  %o3, %o5
 	bcs  got_result
 	mov  %g0, %o2
 	sethi  %hi(0x8000000), %g1
 	cmp  %o3, %g1
 	bcs  not_really_big
 	mov  %g0, %o4
 	cmp  %o5, %g1
 	bcc  not_too_big
 	mov  1, %g2
 	sll  %o5, 4, %o5
 	b  ready_to_divide+0x34
 	inc  %o4
 	addcc  %o5, %o5, %o5
 	bcc  not_too_big
 	inc  %g2
 	sll  %g1, 4, %g1
 	srl  %o5, 1, %o5
 	add  %o5, %g1, %o5
 	b  do_single_div
 	dec  %g2

not_too_big :
 	cmp  %o5, %o3
 	bcs  ready_to_divide+0x4c
 	nop
 	be  do_single_div
 	nop

do_single_div :
 	deccc  %g2
 	bl  end_regular_divide
 	nop
 	sub  %o3, %o5, %o3
 	mov  1, %o2
 	b  end_single_divloop
 	nop

single_divloop :
 	sll  %o2, 1, %o2
 	bl  single_divloop+0x18
 	srl  %o5, 1, %o5
 	sub  %o3, %o5, %o3
 	b  end_single_divloop
 	inc  %o2
 	add  %o3, %o5, %o3
 	dec  %o2

end_single_divloop :
	deccc  %g2
	bge  single_divloop
	tst  %o3
	b,a   end_regular_divide

not_really_big :
 	sll  %o5, 4, %o5
 	cmp  %o5, %o3
 	bleu  not_really_big
 	inccc  %o4
 	be  got_result
 	dec  %o4
 	tst  %o3

divloop :
 	sll  %o2, 4, %o2
 	bl  L1.16
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	bl  L2.17
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	bl  L3.19
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	bl  L4.23
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, 0xf, %o2

L4.23 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, 0xd, %o2

L3.19 :
 	addcc  %o3, %o5, %o3
 	bl  L4.21
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, 0xb, %o2

L4.21 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, 9, %o2

L2.17 :
 	addcc  %o3, %o5, %o3
 	bl  L3.17
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	bl  L4.19
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, 7, %o2

L4.19 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, 5, %o2

L3.17 :
 	addcc  %o3, %o5, %o3
 	bl  L4.17
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, 3, %o2

L4.17 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	inc  %o2

L1.16 :
 	addcc  %o3, %o5, %o3
 	bl  L2.15
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	bl  L3.15
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	bl  L4.15
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -1, %o2

L4.15 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -3, %o2

L3.15 :
 	addcc  %o3, %o5, %o3
 	bl  L4.13
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -5, %o2

L4.13 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -7, %o2

L2.15 :
 	addcc  %o3, %o5, %o3
 	bl  L3.13
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	bl  L4.11
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -9, %o2

L4.11 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -11, %o2

L3.13 :
 	addcc  %o3, %o5, %o3
 	bl  L4.9
 	srl  %o5, 1, %o5
 	subcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -13, %o2

L4.9 :
 	addcc  %o3, %o5, %o3
 	b  end_regular_divide
 	add  %o2, -15, %o2

end_regular_divide :
 	deccc  %o4
 	bge  divloop
 	tst  %o3
 	bl,a   got_result
 	dec  %o2

got_result :
 	tst  %g3
 	bl,a   got_result+0xc
 	neg  %o2
 	retl
 	mov  %o2, %o0

