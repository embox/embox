#TODO !!!!!!!!!!in this version folder tests must set lat in list its makefile diff from other
SUBDIRS:=mem wmem lspnp
OBJS:=test_irq_availability.o test_cpu_context.o

.PHONY: all $(SUBDIRS)

create_objs_lst:$(SUBDIRS)
	echo '$(addprefix $(CURDIR)/,$(OBJS)) \' >> $(ROOT_DIR)/objs.lst
#TODO !!!!!!!!!!in this version folder tests must set lat in list its makefile diff from other
	echo ' -I$(CURDIR)' >> $(ROOT_DIR)/include_dirs.lst

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKEOP)


all:$(SUBDIRS) $(OBJS)

%.o:%.S
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DSIMULATE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

%.o:%.c
	$(CC) $(CCFLAGS)  $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DSIMULATE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

clean:
	rm -rf $(OBJS) *.d

