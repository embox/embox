$_PACKAGE := embox.kernel

$_APIS += tasks_api

$_MODS += tasks no_tasks
$_SRCS-tasks += task.c file.c
$_PROVIDES-tasks += embox.kernel.tasks_api
$_DEPS-tasks += embox.driver.virtual.diag_device
