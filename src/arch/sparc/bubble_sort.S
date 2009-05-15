/**
 * \file bubble_sort.S
 *
 * \date May 15, 2009
 * \author afomin
 */

 	.section    ".text"
	.align  4

 	.global bubble_sort

	.local outer_for
	.local inner_for
	.local dont_swap
	.local continue

 	/**
	 * Sort function (0-end array)
	 * Destroys %l3-%l7
	 * @param %o0 - pointer to array to be sorted
	 */
bubble_sort:
//	for (i = size - 1; i > 0; i--) {						-- outer_for_1
//		for (j = i; j > 0; j--) {							-- inner_for_1
//			if (a[j] < a[j - 1])
//				swap(a[j], a[j - 1]);
//		}													-- inner_for_end_1
//	}														-- continue_1
//
// %l3 - size
// %l4 - tmp
// %l5 - i
// %l6 - j
// %l7 - tmp

	set 0, %l3 		// size = 0;
count_size:
	ld [%l3 + %o0], %l4
	cmp %l4, 0
	bne count_size
	 inc %l3

	dec %l3		// Now %l3 = size


	mov %l3, %l5
	dec %l5 				// i = SIZE - 1

	cmp	%l5, 0				// if (i <= 0)
	ble continue			//   break (outer)
	 nop
outer_for:
	mov %l5, %l6 			// j = i (i > 0)
inner_for:
	ld [%o0 + %l6], %l7
	dec %l6
	ld [%o0 + %l6], %l4
	inc %l6

	cmp %l4, %l7			// if (TTB[j - 2] <= TTB[j - 1])
	ble dont_swap			//   Don't swap
	 nop
	st %l4, [%o0 + %l6] // swapping
	dec %l6
	st %l7, [%o0 + %l6]
	inc %l6
dont_swap:
	dec %l6 				// j--
	cmp	%l6, 0				// if (j > 0)
	bg inner_for			//   continue (inner)
	 nop

	dec %l5 				// i--
	cmp	%l5, 0				// if (i > 0)
	bg outer_for			//   continue (outer)
	 nop

continue:
	jmpl %r15 + 8, %g0
	 nop
