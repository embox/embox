OBJS:=traps.o boot.o main.o mem_traps.o irq.o test.o mul_div.o urem.o

all:$(OBJS)

%.o:%.S
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<

%.o:%.c
	$(CC) $(CCFLAGS)  $(INCLUDE_DIRS) -o $(OBJ_DIR)/$@ $<

clean:
	rm -rf $(OBJS) *.d


