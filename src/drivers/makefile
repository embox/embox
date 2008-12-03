OBJS:=uart.o timers.o

all:$(OBJS)

%.o:%.S
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DRELEASE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

%.o:%.c
	$(CC) $(CCFLAGS)  $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<
	$(CC) $(CCFLAGS) -DRELEASE $(INCLUDE_DIRS) -o $(OBJ_DIR_SIM)/$@ $<

clean:
	rm -rf $(OBJS) *.d
