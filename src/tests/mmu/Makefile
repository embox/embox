
$_MODS += mmu_enable mmu_rw mmu_dfault mmu_contexts mmu_pagefault mmu_write_protection

$_SRCS-mmu_enable += mmu_enable.c
$_SRCS-mmu_rw += mmu_rw.c
$_SRCS-mmu_dfault += mmu_dfault.c
$_SRCS-mmu_contexts += mmu_contexts.c
$_SRCS-mmu_pagefault += mmu_pagefault.c
$_SRCS-mmu_write_protection += mmu_write_protection.c

$_DEPS-mmu_enable += embox.hal.test.mmu
$_DEPS-mmu_enable += embox.hal.mm.mmu
$_DEPS-mmu_rw += embox.hal.test.mmu
$_DEPS-mmu_rw += embox.hal.mm.mmu
$_DEPS-mmu_dfault += embox.hal.test.mmu
$_DEPS-mmu_dfault += embox.hal.mm.mmu
$_DEPS-mmu_dfault += embox.hal.traps
$_DEPS-mmu_dfault += embox.hal.test.traps
$_DEPS-mmu_pagefault += embox.hal.test.mmu
$_DEPS-mmu_pagefault += embox.hal.mm.mmu
$_DEPS-mmu_pagefault += embox.hal.traps
$_DEPS-mmu_pagefault += embox.hal.test.traps
$_DEPS-mmu_contexts += embox.hal.test.mmu
$_DEPS-mmu_contexts += embox.hal.mm.mmu

$_DEPS-mmu_write_protection += embox.hal.test.mmu
$_DEPS-mmu_write_protection += embox.hal.test.traps
$_DEPS-mmu_write_protection += embox.hal.mm.mmu
$_DEPS-mmu_write_protection += embox.hal.traps
