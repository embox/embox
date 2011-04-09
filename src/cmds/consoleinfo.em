$_MODS += consoleinfo
$_SRCS-consoleinfo += consoleinfo.c
$_DEPS-consoleinfo += embox.driver.vconsole

$_BRIEF-consoleinfo = Print information about current console

define $_DETAILS-consoleinfo
	NAME
		consoleinfo

	AUTHORS
		Fedor Burdun
endef
