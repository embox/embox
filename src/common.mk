.PHONY: all $(SUBDIRS-y) $(SUBDIRS-n)

create_objs_lst: $(SUBDIRS-y)
	echo '$(addprefix $(CURDIR)/,$(OBJS-y)) \' >> $(ROOT_DIR)/objs.lst

create_include_dirs_lst: $(SUBDIRS-y) $(SUBDIRS-n)
	echo ' -I$(CURDIR)\' >> $(ROOT_DIR)/include_dirs.lst

$(SUBDIRS-y):
	$(MAKE) -C $@ $(MAKEOP)

$(SUBDIRS-n):
	$(MAKE) -C $@ $(MAKEOP)

all: $(SUBDIRS-y) $(OBJS-y)

%.o:%.S
	$(CC) $(CCFLAGS) -DDEBUG_TRG $(INCLUDE_DIRS) -o $(OBJ_DIR_DBG)/$@ $<
	$(CC) $(CCFLAGS) -DRELEASE_TRG $(INCLUDE_DIRS) -o $(OBJ_DIR_RLS)/$@ $<
	$(CC) $(CCFLAGS) -DSIMULATION_TRG $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

%.o:%.c
	$(CC) $(CCFLAGS) -DDEBUG_TRG $(INCLUDE_DIRS) -o $(OBJ_DIR_DBG)/$@ $<
	$(CC) $(CCFLAGS) -DRELEASE_TRG $(INCLUDE_DIRS) -o $(OBJ_DIR_RLS)/$@ $<
	$(CC) $(CCFLAGS) -DSIMULATION_TRG $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

clean:
	rm -rf $(OBJS-y) *.d
