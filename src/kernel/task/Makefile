$_PACKAGE := embox.kernel

$_APIS += tasks_api

$_MODS += task_common
$_SRCS-task_common += task_common.c file.c
$_DEPS-task_common += embox.fs.core embox.fs.rootfs

$_MODS += tasks
$_SRCS-tasks += task.c
$_DEPS-tasks += embox.driver.virtual.diag_device task_common
$_REQUIRES-task += embox.kernel.thread.sched_api
$_PROVIDES-tasks += embox.kernel.tasks_api

$_MODS += no_tasks
$_SRCS-no_tasks += no_task.c
$_DEPS-no_tasks += embox.driver.virtual.diag_device task_common
$_PROVIDES-tasks += embox.kernel.tasks_api
