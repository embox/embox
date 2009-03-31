SUBDIRS:=
OBJS:=net_device.o arp.o eth.o net_pack_manager.o ip_v4.o ipaux.o icmp.o socket.o udp.o

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
