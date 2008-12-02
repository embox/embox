SUBDIRS = kernel conio drivers common tests
export INCLUDE_DIRS := $(addprefix -I$(SRC_DIR)/,$(SUBDIRS))


OBJS:=
.PHONY: all $(SUBDIRS)


#VPATH := $(SUBDIRS)

all: release

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKEOP) LOCAL_DIR=$(ROOT_DIR)/$@

release:$(SUBDIRS)
	#TODO have to set entry point in linker skript now importen link order
	$(CC) $(LDFLAGS) -T $(ROOT_DIR)/linkram  -o $(BIN_DIR)/$(TARGET)_ram \
	$(OBJ_DIR)/traps.o $(filter-out $(OBJ_DIR)/traps.o, $(wildcard $(OBJ_DIR)/*.o))
	$(CC) $(LDFLAGS) -T $(ROOT_DIR)/linkrom  -o $(BIN_DIR)/$(TARGET)_rom \
	$(OBJ_DIR)/traps.o $(filter-out $(OBJ_DIR)/traps.o, $(wildcard $(OBJ_DIR)/*.o))
	$(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_ram > $(BIN_DIR)/$(TARGET)_ram.dis


clean:$(SUBDIRS)

