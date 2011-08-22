
$_PACKAGE := embox.kernel

$_LDFLAGS  += $($_SELFDIR)/nosys.lds

$(IMAGE): $($_SELFDIR)/nosys.lds

$_MODS_CORE += main

$_SRCS-main += init.c
$_SRCS-main += nosys.c
$_DEPS-main += embox.hal.arch

# TODO For what?
$_REQUIRES-main += softirq_api
$_REQUIRES-main += embox.kernel.sched_api

ifdef CONFIG_MMU_SUPPORT
$_DEPS-main += embox.hal.mm.mmu
endif

ifdef CONFIG_WATCHDOG
  $_DEPS-main += embox.driver.abstract.watchdog
endif

ifdef CONFIG_MEASURE
  $_MODS += measure
  $_SRCS-measure += measure.c
  $_DEPS-measure += $(CONFIG_MEASURE) timer
  $_DEPS-main += measure
endif

# Include em-files from current directory.

SELFDIR ?= .
__CMD_EM_FILES := $(wildcard $(SELFDIR)/*.em)

include $(__CMD_EM_FILES)
