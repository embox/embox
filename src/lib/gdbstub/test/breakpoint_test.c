/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 18.08.23
 */
#include <debug/gdbstub.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("test tests for <debug/gdbstub.h>");

static void breakpoint_trigger(void);
static void sw_breakpoint_handler(struct gdb_regs *regs);
static void hw_breakpoint_handler(struct gdb_regs *regs);

TEST_CASE("software breakpoints") {
	if (gdb_enable_bpts(GDB_BPT_TYPE_SOFT)) {
		gdb_set_handler(sw_breakpoint_handler);

		gdb_set_bpt(GDB_BPT_TYPE_SOFT, breakpoint_trigger, 0);
		gdb_activate_all_bpts();

		breakpoint_trigger();

		gdb_disable_bpts(GDB_BPT_TYPE_SOFT);

		test_assert_emitted("aba");
	}
}

TEST_CASE("hardware breakpoints") {
	if (gdb_enable_bpts(GDB_BPT_TYPE_HARD)) {
		gdb_set_handler(hw_breakpoint_handler);

		gdb_set_bpt(GDB_BPT_TYPE_HARD, breakpoint_trigger, 0);
		gdb_activate_all_bpts();

		breakpoint_trigger();

		gdb_disable_bpts(GDB_BPT_TYPE_HARD);

		test_assert_emitted("aba");
	}
}

static void breakpoint_trigger(void) {
	test_emit('a');
}

static void sw_breakpoint_handler(struct gdb_regs *regs) {
	gdb_deactivate_all_bpts();
	{
		breakpoint_trigger();
		gdb_remove_bpt(GDB_BPT_TYPE_SOFT, GDB_REGS_PC(regs), 0);
	}
	gdb_activate_all_bpts();

	test_emit('b');
}

static void hw_breakpoint_handler(struct gdb_regs *regs) {
	gdb_deactivate_all_bpts();
	{
		breakpoint_trigger();
		gdb_remove_bpt(GDB_BPT_TYPE_HARD, GDB_REGS_PC(regs), 0);
	}
	gdb_activate_all_bpts();

	test_emit('b');
}
