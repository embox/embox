/*#define bas_addr %l0
#define t_index %l1
#define test_offset %l2
// TODO
// base_addr, addr_mask, test_offset, t_index, t_value

addr_bus_test:
	set base_addr, t0;
	mov %g0, t_index;
set_memory_loop:
	st PATTERN, [base_addr + t_index];
	sll t_index, 1, t_index;
	and t_index, addr_mask, t_index;
	cmp t_index, %g0;
	bne set_memory_loop;
	 nop;

	mov %g0, test_offset;
test_memory_loop:
	st ANTIPATTERN, [base_addr + test_offset];
	mov %g0, t_index;
	cmp t_index, test_offset;
	bge less_ok ## IDENTIFIER;
	 nop;

check_less_loop:
	ld [base_addr + t_index], t_value;
	cmp t_value, PATTERN;
	bne fail_loop ## IDENTIFIER;
	 nop;
	sll t_index, 1, t_index;
	cmp t_index, test_offset;
	blu check_less_loop;
	 nop;

less_ok:
	ld [base_addr + t_index], t_value;
	cmp t2, ANTIPATTERN;
	be less_or_equal_ok;
	 nop;
less_or_equal_ok:
	and t_index, addr_mask, t_index;
	cmp t_index, %g0;
	be check_ok ## IDENTIFIER;
	 nop;

check_higher_loop:
	ld [base_addr + t_index], t_value;
	cmp t_value, PATTERN;
	bne fail_loop ## IDENTIFIER;
	 nop;
	sll t_index, 1, t_index;
	and t_index, addr_mask, t_index;
	cmp t_index, %g0;
	be check_ok;
	 nop;

check_ok:
	st PATTERN, [base_addr + test_offset];
	sll test_offset, 1, test_offset;
	and test_offset, addr_mask, test_offset;
	cmp test_offset, %g0;
	bne test_memory_loop;
	 nop;
	PUTC('+')
	// TODO exit from function

fail_loop:
	PUTC('y');
	ba fail_loop;
	 nop;
*/
