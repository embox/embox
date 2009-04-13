	.section    ".text"
	.align  4
!!!!call copy addr in %r15(%o7)
	.global test_save_trap
test_save_trap:

	set test_cpu_context_buff, %o0
	! store previous nPC as PC
	mov		%l2, %l1
	! store previous nPC+4 as nPC
	add     %l2, 4, %l2
test_ggfdfgdf:
	set		test_ggfdfgdf, %o1
	add  %o1,20, %o1
	add %o1, 4, %o2
	call save_proc_context
	nop
test_save_trap_cont:
	nop
test_save_trap_exit:
	jmp  %l1
	rett  %l2

	.global test_restore_trap
test_restore_trap:
	set test_cpu_context_buff, %o0
	call restore_proc_context
	nop
	//exit not here
	jmp  %o1
	rett  %o2
.align  32

