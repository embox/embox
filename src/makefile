include $(SCRIPTS_DIR)/autoconf

SUBDIRS-$(MONITOR_ARCH)        += arch
SUBDIRS-$(MONITOR_KERNEL)      += kernel
SUBDIRS-$(MONITOR_DRIVERS)     += drivers
SUBDIRS-$(MONITOR_MISC)        += misc
SUBDIRS-$(MONITOR_TESTS_EXEC)  += tests
SUBDIRS-$(MONITOR_FS)          += fs
ifeq ($(SIMULATION_TRG),n)
SUBDIRS-$(MONITOR_CONIO)       += conio
SUBDIRS-$(MONITOR_USER)        += user
SUBDIRS-$(MONITOR_NET)         += net
else
SUBDIRS-n                      += conio
SUBDIRS-n                      += user
SUBDIRS-n                      += net
endif

#export INCLUDE_DIRS:=$(addprefix -I$(SRC_DIR)/,$(SUBDIRS))

.PHONY: all release debug simulation docs $(SUBDIRS-y) $(SUBDIRS-n)

all: $(ALL_TARGETS)

create_objs_lst: $(SUBDIRS-y)
create_include_dirs_lst: $(SUBDIRS-y) $(SUBDIRS-n)

$(SUBDIRS-y):
ifdef G_DIRS
	@declare -x INCLUDE_DIRS=$(filter-out '\\','$(G_DIRS)'); $(MAKE) -C $@ $(MAKEOP)
else
	@$(MAKE) -C $@ $(MAKEOP)
endif

$(SUBDIRS-n):
	@$(MAKE) -C $@ $(MAKEOP)

release: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#TODO have to set entry point in linker script now importen link order
	@#echo gobjs $(GOBJS)
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linkrom  -o $(BIN_DIR)/$(TARGET)_rom \
	$(OBJ_DIR_RLS)/traps.o $(filter-out $(OBJ_DIR_RLS)/traps.o, $(wildcard $(OBJ_DIR_RLS)/*.o))
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_rom > $(BIN_DIR)/$(TARGET)_rom.dis; \
	else \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_rom > /dev/null; \
	fi;

debug: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#TODO have to set entry point in linker script now importen link order
	@#echo gobjs $(GOBJS)
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linkram  -o $(BIN_DIR)/$(TARGET)_ram \
	$(OBJ_DIR_DBG)/traps.o $(filter-out $(OBJ_DIR_DBG)/traps.o, $(wildcard $(OBJ_DIR_DBG)/*.o))
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_ram > $(BIN_DIR)/$(TARGET)_ram.dis; \
	fi;

simulation: $(SUBDIRS-y)
	@#TODO may be use GOBJS list of objects files
	@#echo gobjs $(GOBJS)
	@#TODO have to set entry point in linker script now importen link order
	@$(CC) $(LDFLAGS) -T $(SCRIPTS_DIR)/linksim  -o $(BIN_DIR)/$(TARGET)_sim \
	$(OBJ_DIR_SIM)/traps.o $(filter-out $(OBJ_DIR_SIM)/traps.o, $(wildcard $(OBJ_DIR_SIM)/*.o))
	@if [ $(DISASSEMBLE) == y ]; \
	then \
	    $(OD_TOOL) -S $(BIN_DIR)/$(TARGET)_sim > $(BIN_DIR)/$(TARGET)_sim.dis; \
	fi;
	@$(OC_TOOL) -O srec $(BIN_DIR)/$(TARGET)_sim $(BIN_DIR)/prom.srec

docs:
	@cd .. && doxygen

clean: $(SUBDIRS-y)
