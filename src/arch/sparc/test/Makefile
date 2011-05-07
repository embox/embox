$_PACKAGE := embox.hal.test

$_MODS += mmu irq traps

$_SRCS-mmu += testmmu_core.c
$_SRCS-irq += testirq_core.c
$_SRCS-traps += testtrap_handler.c testtrap_entry.S testtrap_table.S testtraps_core.c

$_DEPS-mmu += embox.hal.mm.mmu
$_DEPS-traps += embox.hal.traps
