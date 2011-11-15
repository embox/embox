$_PACKAGE := org.lua

$_CPPFLAGS += -I$(SRC_DIR)/include/posix -I$(SRC_DIR)/include/util

$_MODS += lua_core
$_SRCS-lua_core += lapi.c lauxlib.c lbaselib.c lcode.c
$_SRCS-lua_core += ldebug.c ldo.c ldump.c lfunc.c lgc.c linit.c
$_SRCS-lua_core += llex.c lmem.c lobject.c lopcodes.c
$_SRCS-lua_core += lparser.c lstate.c lstring.c
$_SRCS-lua_core += ltable.c ltm.c lundump.c lvm.c lzio.c

$_ROOTFS_SRCS := $(SELFDIR)/fib.lua
