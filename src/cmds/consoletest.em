$_MODS += consoletest
$_SRCS-consoletest += consoletest.c
$_DEPS-consoletest += embox.driver.vconsole

$_BRIEF-consoletest = test console

define $_DETAILS-consoletest
	NAME
		consoletest

	AUTHORS
		Fedor Burdun
endef
