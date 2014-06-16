/**
 * @file
 *
 * @data May 7, 2014
 * @author Anton Bondarev
 */


.global cpu_stack_move
.type cpu_stack_move, %function

.text
cpu_stack_move:
    b 	    cpu_stack_move
