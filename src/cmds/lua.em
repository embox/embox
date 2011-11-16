$_MODS += lua
$_SRCS-lua += lua.c
$_CPPFLAGS-lua += -I$(THIRDPARTY_DIR)/lua
$_DEPS-lua += org.lua.lua_core

$_BRIEF-lua = Lua interpreter

define $_DETAILS-lua
	NAME
		lua - Lua interpreter
	SYNOPSIS
		lua [script]
	AUTHORS
		Nikolay Korotkiy
endef
