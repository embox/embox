export
ROOT_DIR := $(CURDIR)

BIN_DIR:=$(ROOT_DIR)/bin
OBJ_DIR:=$(ROOT_DIR)/obj
SRC_DIR:=$(ROOT_DIR)/src

#name of target
TARGET := monitor
#compiler
CC_PACKET := sparc-elf
#tools
CC :=$(CC_PACKET)-gcc

OD_TOOL :=$(CC_PACKET)-objdump
OC_TOOL :=$(CC_PACKET)-objcopy

#compiler flags (+optimiz +debug_info)
CCFLAGS := -Wall -msoft-float -c -MD -O0 -DLEON3
#link flags
LDFLAGS:= -Wl -N -nostdlib

OBJS:=

all:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	declare -x MAKEOP=all; make --directory=src all

clean:
	declare -x MAKEOP=clean; make --directory=src clean
	rm -rf $(BIN_DIR) $(OBJ_DIR)
