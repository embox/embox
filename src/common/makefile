SUBDIRS:=
OBJS:=common.o string.o

create_objs_lst:$(SUBDIRS)
	echo '$(addprefix $(CURDIR)/,$(OBJS)) \' >> $(ROOT_DIR)/objs.lst
	echo ' -I$(CURDIR)\' >> $(ROOT_DIR)/include_dirs.lst

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKEOP)


all:$(SUBDIRS) $(OBJS)

%.o:%.S
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DRELEASE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

%.o:%.c
	$(CC) $(CCFLAGS)  $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DRELEASE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

clean:
	rm -rf $(OBJS) *.d
