ifdef CONFIG_PP_ENABLE
$_PACKAGE := core.kernel
$_MODS += pp

# for variable cur_console
$_DEPS-pp += embox.driver.vconsole

ifdef CONFIG_PP_TEST
$_DEPS-pp += #embox.test.pp_test
endif

$_SRCS-pp += pp.c
endif
