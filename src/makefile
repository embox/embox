#TODO !!!!!!!!!!in this version folder tests must set lat in list its makefile diff from other
SUBDIRS = arch net kernel conio drivers common tests
OBJS =
#export INCLUDE_DIRS:=$(addprefix -I$(SRC_DIR)/,$(SUBDIRS))



.PHONY: all $(SUBDIRS)

all: simulation release debug

create_objs_lst:$(SUBDIRS)


$(SUBDIRS):
ifdef G_DIRS
	declare -x INCLUDE_DIRS=$(filter-out '\\','$(G_DIRS)');$(MAKE) -C $@ $(MAKEOP)
else
	$(MAKE) -C $@ $(MAKEOP)
endif

release:$(SUBDIRS)
	#TODO may be use GOBJS list of objects files
	#TODO have to set entry point in linker script now importen link order
	#@echo gobjs $(GOBJS)
	$(CC) $(LDFLAGS) -T $(ROOT_DIR)/linkrom  -o $(BIN_DIR)/$(TARGET)_rom \
	$(OBJ_DIR)/traps.o $(filter-out $(OBJ_DIR)/traps.o, $(wildcard $(OBJ_DIR)/*.o))
	$(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_rom > $(BIN_DIR)/$(TARGET)_rom.dis


debug:$(SUBDIRS)
	#TODO may be use GOBJS list of objects files
	#TODO have to set entry point in linker script now importen link order
	#@echo gobjs $(GOBJS)
	$(CC) $(LDFLAGS) -T $(ROOT_DIR)/linkram  -o $(BIN_DIR)/$(TARGET)_ram \
	$(OBJ_DIR)/traps.o $(filter-out $(OBJ_DIR)/traps.o, $(wildcard $(OBJ_DIR)/*.o))
	$(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_ram > $(BIN_DIR)/$(TARGET)_ram.dis

simulation:$(SUBDIRS)
	#TODO may be use GOBJS list of objects files
	#@echo gobjs $(GOBJS)
	#TODO have to set entry point in linker script now importen link order
	$(CC) $(LDFLAGS) -T $(ROOT_DIR)/linksim  -o $(BIN_DIR)/$(TARGET)_sim \
	$(OBJ_DIR)/traps.o $(filter-out $(OBJ_DIR_SIM)/traps.o, $(wildcard $(OBJ_DIR_SIM)/*.o))
	$(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_sim > $(BIN_DIR)/$(TARGET)_sim.dis
	$(OC_TOOL) -O srec $(BIN_DIR)/$(TARGET)_sim $(BIN_DIR)/prom.srec


clean:$(SUBDIRS)

