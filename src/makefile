include $(SCRIPTS_DIR)/autoconf

SUBDIRS-y                      += arch
SUBDIRS-y                      += kernel
SUBDIRS-y                      += drivers
SUBDIRS-y                      += lib
SUBDIRS-$(MONITOR_TESTS)       += tests
SUBDIRS-$(MONITOR_FS)          += fs
SUBDIRS-y                      += user
SUBDIRS-$(MONITOR_NETWORK)     += net

#export INCLUDE_DIRS:=$(addprefix -I$(SRC_DIR)/,$(SUBDIRS))

OBJ_HEAD:= entry.o


.PHONY: all release debug sim docs $(SUBDIRS-y) $(SUBDIRS-n)

all: $(BUILD)

create_objs_lst: $(SUBDIRS-y)
create_include_dirs_lst: $(SUBDIRS-y) $(SUBDIRS-n)

$(SUBDIRS-y):
ifdef G_DIRS
	@declare -x INCLUDE_DIRS=$(filter-out '\\','$(G_DIRS)'); $(MAKE) --no-print-directory -C $@ $(MAKEOP)
else
	@$(MAKE) --no-print-directory -C $@ $(MAKEOP)
endif

$(SUBDIRS-n):
	@$(MAKE) --no-print-directory -C $@ $(MAKEOP)

release: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#TODO have to set entry point in linker script now importen link order
	@#echo gobjs $(GOBJS)
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linkrom  -o $(BIN_DIR)/$(TARGET)_rom \
	$(OBJ_DIR)/$(BUILD)/$(OBJ_HEAD) $(filter-out $(OBJ_DIR)/$(BUILD)/$(OBJ_HEAD), $(wildcard $(OBJ_DIR)/$(BUILD)/*.o))
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
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linkram  -o $(BIN_DIR)/$(TARGET)_ram \
	$(OBJ_DIR)/$(BUILD)/$(OBJ_HEAD) $(filter-out $(OBJ_DIR)/$(BUILD)/$(OBJ_HEAD), $(wildcard $(OBJ_DIR)/$(BUILD)/*.o))
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_ram > $(BIN_DIR)/$(TARGET)_ram.dis; \
	fi;
	@$(OC_TOOL) -O srec $(BIN_DIR)/$(TARGET)_ram $(BIN_DIR)/prom.srec

sim: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#echo gobjs $(GOBJS)
	@#TODO have to set entry point in linker script now importen link order
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linksim  -o $(BIN_DIR)/$(TARGET)_sim \
	$(OBJ_DIR)/$(BUILD)/$(OBJ_HEAD) $(filter-out $(OBJ_DIR)/$(BUILD)/$(OBJ_HEAD), $(wildcard $(OBJ_DIR)/$(BUILD)/*.o))
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_sim > $(BIN_DIR)/$(TARGET)_sim.dis; \
	fi;
	@$(OC_TOOL) -O srec $(BIN_DIR)/$(TARGET)_sim $(BIN_DIR)/prom.srec

docs:
	@cd .. && doxygen

clean: $(SUBDIRS-y)
