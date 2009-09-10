export
ROOT_DIR   := $(CURDIR)
BIN_DIR     = $(ROOT_DIR)/bin
OBJ_DIR     = $(ROOT_DIR)/obj
SRC_DIR     = $(ROOT_DIR)/src
SCRIPTS_DIR = $(ROOT_DIR)/scripts
INCLUDE_DIR = $(ROOT_DIR)/include
RM          = rm -rf
EDITOR      = vim

ifeq ($(shell [ -f scripts/autoconf ] && echo YES),YES)
    DEFAULT_CONF =
else
    DEFAULT_CONF = .in
endif

include $(SCRIPTS_DIR)/autoconf$(DEFAULT_CONF)

ifeq ($(SIMULATION_TRG),y)
BUILD = sim
endif
ifeq ($(DEBUG_TRG),y)
BUILD = debug
endif
ifeq ($(RELEASE_TRG),y)
BUILD = release
endif
ifeq ($(DOXYGEN_TRG),y)
BUILD = docs
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
	@test -d $(BIN_DIR)          || mkdir -p $(BIN_DIR)
	@test -d $(OBJ_DIR)          || mkdir -p $(OBJ_DIR)
	@test -d $(OBJ_DIR)/$(BUILD) || mkdir -p $(OBJ_DIR)/$(BUILD)

build:
	@rm -f objs.lst include_dirs.lst
	@declare -x MAKEOP=create_objs_lst; $(MAKE) --no-print-directory --directory=src create_objs_lst
	@declare -x MAKEOP=create_include_dirs_lst; $(MAKE) --no-print-directory --directory=src create_include_dirs_lst
	@echo ' ' >> $(ROOT_DIR)/include_dirs.lst
	@declare -x MAKEOP=all G_DIRS=`cat include_dirs.lst`; $(MAKE) --no-print-directory --directory=src all

checksum:
	@if [ $(SIGN_CHECKSUM) == y ]; \
	then \
	    $(SCRIPTS_DIR)/checksum.py -o $(OC_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD); \
	    declare -x MAKEOP=all G_DIRS=`cat include_dirs.lst`; $(MAKE) --no-print-directory --directory=src all; \
	else \
	    $(SCRIPTS_DIR)/checksum.py -o $(OC_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean; \
	fi;

clean:
	@declare -x MAKEOP=clean; $(MAKE) --no-print-directory --directory=src clean
	@$(RM) $(BIN_DIR) $(OBJ_DIR) objs.lst include_dirs.lst .config.old docs/
	@$(SCRIPTS_DIR)/checksum.py -o $(OD_TOOL) -d $(BIN_DIR) -t $(TARGET) --build=$(BUILD) --clean

clean_all: clean
	@$(RM) .config $(SCRIPTS_DIR)/autoconf $(SCRIPTS_DIR)/autoconf.h src/conio/shell.inc src/conio/users.inc
	@ln -sf -T asm-sparc include/asm

xconfig:
	@$(SCRIPTS_DIR)/configure.py --mode=x

menuconfig:
	@$(EDITOR) $(SCRIPTS_DIR)/autoconf
	@$(SCRIPTS_DIR)/configure.py --mode=menu

config:
	@echo "Oops! Try edit config file by hand or use \"make x(menu)config\" and have a lot of fun."
