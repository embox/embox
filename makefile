export
ROOT_DIR   := $(CURDIR)
BIN_DIR     = $(ROOT_DIR)/bin
OBJ_DIR     = $(ROOT_DIR)/obj
OBJ_DIR_SIM = $(OBJ_DIR)/sim
OBJ_DIR_DBG = $(OBJ_DIR)/debug
OBJ_DIR_RLS = $(OBJ_DIR)/release
SRC_DIR     = $(ROOT_DIR)/src
SCRIPTS_DIR = $(ROOT_DIR)/scripts
ifeq ($(shell find scripts -name autoconf -print), "scripts/autoconf")
include $(SCRIPTS_DIR)/autoconf
else
include $(SCRIPTS_DIR)/autoconf.default
endif
ifeq ($(SIMULATION_TRG),y)
BUILD += --sim
endif
ifeq ($(DEBUG_TRG),y)
BUILD += --debug
endif
ifeq ($(RELEASE_TRG),y)
BUILD += --release
endif

CC      = $(CC_PACKET)-gcc
OD_TOOL = $(CC_PACKET)-objdump
OC_TOOL = $(CC_PACKET)-objcopy

.PHONY: mkdir build checksum all clean config xconfig menuconfig

all: mkdir build checksum

mkdir:
	@if [ -e .config ]; \
	then \
	    echo Start; \
	else \
	    echo "Try make x(menu)config before"; exit 1;\
	fi;
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	@test -d $(OBJ_DIR) || mkdir -p $(OBJ_DIR)
	@test -d $(OBJ_DIR)/sim || mkdir -p $(OBJ_DIR)/sim
	@test -d $(OBJ_DIR)/debug || mkdir -p $(OBJ_DIR)/debug
	@test -d $(OBJ_DIR)/release || mkdir -p $(OBJ_DIR)/release

build:
	@rm -f objs.lst include_dirs.lst
	@declare -x MAKEOP=create_objs_lst; make --directory=src create_objs_lst
	@declare -x MAKEOP=create_include_dirs_lst; make --directory=src create_include_dirs_lst
	@echo ' ' >> $(ROOT_DIR)/include_dirs.lst
	@declare -x MAKEOP=all G_DIRS=`cat include_dirs.lst`; make --directory=src all

checksum:
	@if [ $(SIGN_CHECKSUM) == y ]; \
	then \
	    $(SCRIPTS_DIR)/checksum.py -o $(OD_TOOL) -d $(BIN_DIR) -t $(TARGET) $(BUILD); \
	    declare -x MAKEOP=all G_DIRS=`cat include_dirs.lst`; make --directory=src all; \
	fi;

clean:
	@declare -x MAKEOP=clean; make --directory=src clean
	@rm -rf $(BIN_DIR) $(OBJ_DIR) objs.lst include_dirs.lst .config.old

xconfig:
	@$(SCRIPTS_DIR)/configure.py --mode=x

menuconfig:
	@vim $(SCRIPTS_DIR)/autoconf
	@$(SCRIPTS_DIR)/configure.py --mode=menu

config:
	@echo "Oops! Try edit config file by hand or use \"make x(menu)config\" and have a lot of fun."
