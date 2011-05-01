$_PACKAGE = embox.test.kernel.thread

$_MODS           += state_test
$_SRCS-state_test = state_test.c

$_MODS           += sched_test
$_SRCS-sched_test = sched_test.c
$_DEPS-sched_test = core.kernel.thread.sched

$_MODS           += core_test
$_SRCS-core_test  = core_test.c
$_DEPS-core_test  = core.kernel.thread.core

$_MODS                    += suspend_resume_test
$_SRCS-suspend_resume_test = suspend_resume_test.c
$_DEPS-suspend_resume_test = core.kernel.thread.core

$_MODS                          += priority_inheritance_test
$_SRCS-priority_inheritance_test = priority_inheritance_test.c
$_DEPS-priority_inheritance_test = core.kernel.thread.core
