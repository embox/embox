export
ROOT_DIR := $(CURDIR)
BIN_DIR:= $(ROOT_DIR)/bin
OBJ_DIR:= $(ROOT_DIR)/obj
OBJ_DIR_SIM:= $(OBJ_DIR)/sim
SRC_DIR:= $(ROOT_DIR)/src

#name of target
TARGET:= monitor
#compiler
CC_PACKET:= sparc-elf
#tools
CC:= $(CC_PACKET)-gcc

OD_TOOL:= $(CC_PACKET)-objdump
OC_TOOL:= $(CC_PACKET)-objcopy

#compiler flags (+optimiz +debug_info)
CCFLAGS:= -Werror -msoft-float -c -MD -mv8 -O0 -g -DLEON3 -D_TEST_SYSTEM_ -D_ERROR -D_TRACE
#CCFLAGS_SIMULATE = $(CCFLAGS)-DSIMULATE
#link flags
LDFLAGS:= -Wl -N -nostdlib -g

ifndef CPU_ARCH
CPU_ARCH:= sparc
endif

all:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/sim
	rm -f objs.lst include_dirs.lst
	declare -x MAKEOP=create_objs_lst; make --directory=src create_objs_lst
	echo ' ' >> $(ROOT_DIR)/include_dirs.lst
	uniq src/user/subdirs > src/user/.subdirs
	mv src/user/.subdirs src/user/subdirs
	declare -x MAKEOP=all G_DIRS=`cat include_dirs.lst`; make --directory=src all

clean:
	declare -x MAKEOP=clean; make --directory=src clean
	rm -rf $(BIN_DIR) $(OBJ_DIR) objs.lst include_dirs.lst

xconfig:
	@./scripts/configure.py

config:
	@echo "Oops! Try edit config file by hand or use \"make xconfig\" and have a lot of fun."
