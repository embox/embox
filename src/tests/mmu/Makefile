
$_MODS += mmu_enable mmu_rw mmu_dfault mmu_contexts mmu_pagefault
$_MODS += mmu_write_protection mmu_page_mark_writeable

$_SRCS-mmu_enable += mmu_enable.c
$_SRCS-mmu_rw += mmu_rw.c
$_SRCS-mmu_dfault += mmu_dfault.c
$_SRCS-mmu_contexts += mmu_contexts.c
$_SRCS-mmu_pagefault += mmu_pagefault.c
$_SRCS-mmu_write_protection += mmu_write_protection.c
$_SRCS-mmu_page_mark_writeable += mmu_page_mark_writeable.c


$_DEPS-mmu_enable += embox.hal.test.mmu

$_DEPS-mmu_rw += embox.hal.test.mmu

$_DEPS-mmu_dfault += embox.hal.test.mmu embox.hal.test.traps

$_DEPS-mmu_pagefault += embox.hal.test.mmu embox.hal.test.traps

$_DEPS-mmu_contexts += embox.hal.test.mmu

$_DEPS-mmu_write_protection += embox.hal.test.mmu embox.hal.test.traps

$_DEPS-mmu_page_mark_writeable += embox.hal.test.mmu embox.hal.test.traps
