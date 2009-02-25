SUBDIRS:=
OBJS:=window_overflow_func.o cpu_context.o restore_proc_context.o save_proc_context.o traps.o boot.o main.o mem_traps.o irq.o test.o mul_div.o urem.o tests_iterator.o

create_objs_lst:$(SUBDIRS)
	echo '$(addprefix $(CURDIR)/,$(OBJS)) \' >> $(ROOT_DIR)/objs.lst
	echo ' -I$(CURDIR)\' >> $(ROOT_DIR)/include_dirs.lst

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


