!!!!call copy addr in %r15(%o7)
	.global test_save_trap
test_save_trap:
	call save_proc_context
	nop
test_save_trap_cont:
	call restore_proc_context
	nop
test_save_trap_exit:
	jmp  %l1 + 4
	rett  %l2 + 4

	.global test_restore_trap
test_restore_trap:
	call restore_proc_context
	nop
	jmp  %l1 + 4
	rett  %l2 + 4
	//rett  %l1 + 8
.align  32
