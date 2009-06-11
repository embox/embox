.PHONY: all $(SUBDIRS-y)

create_objs_lst: $(SUBDIRS-y)
	echo '$(addprefix $(CURDIR)/,$(OBJS-y)) \' >> $(ROOT_DIR)/objs.lst
	echo ' -I$(CURDIR)\' >> $(ROOT_DIR)/include_dirs.lst

$(SUBDIRS-y):
	$(MAKE) -C $@ $(MAKEOP)

all: $(SUBDIRS-y) $(OBJS-y)

%.o:%.S
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DSIMULATE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

%.o:%.c
	$(CC) $(CCFLAGS)  $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DSIMULATE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

clean:
	rm -rf $(OBJS-y) *.d
