.PHONY: all release debug sim docs $(SUBDIRS-y) $(SUBDIRS-n)

all: $(BUILD)

$(SUBDIRS-y):
	@$(MAKE) -C $@ $(MAKEOP)

$(SUBDIRS-n):
	@$(MAKE) -C $@ $(MAKEOP)

release: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#TODO have to set entry point in linker script now importen link order
	@#echo gobjs $(GOBJS)
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linker/linkrom  -o $(BIN_DIR)/$(TARGET)_rom \
	$(OBJ_DIR)/$(BUILD)/*.o
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_rom > $(BIN_DIR)/$(TARGET)_rom.dis; \
	else \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_rom > /dev/null; \
	fi;
	@$(OC_TOOL) -O srec $(BIN_DIR)/$(TARGET)_rom $(BIN_DIR)/prom.srec

debug: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#TODO have to set entry point in linker script now importen link order
	@#echo gobjs $(GOBJS)
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linker/linkram  -o $(BIN_DIR)/$(TARGET)_ram \
	$(OBJ_DIR)/$(BUILD)/*.o
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_ram > $(BIN_DIR)/$(TARGET)_ram.dis; \
	fi;
	@$(OC_TOOL) -O srec $(BIN_DIR)/$(TARGET)_ram $(BIN_DIR)/prom.srec

sim: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#echo gobjs $(GOBJS)
	@#TODO have to set entry point in linker script now importen link order
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linker/linksim  -o $(BIN_DIR)/$(TARGET)_sim \
	$(OBJ_DIR)/$(BUILD)/*.o
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_sim > $(BIN_DIR)/$(TARGET)_sim.dis; \
	fi;
	@$(OC_TOOL) -O srec $(BIN_DIR)/$(TARGET)_sim $(BIN_DIR)/prom.srec

docs:
	@cd .. && doxygen

clean: $(SUBDIRS-y)
