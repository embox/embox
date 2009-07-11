.PHONY: all $(SUBDIRS-y) $(SUBDIRS-n)

create_objs_lst: $(SUBDIRS-y)
	@echo '$(addprefix $(CURDIR)/,$(OBJS-y)) \' >> $(ROOT_DIR)/objs.lst

create_include_dirs_lst: $(SUBDIRS-y) $(SUBDIRS-n)
	@echo ' -I$(CURDIR)\' >> $(ROOT_DIR)/include_dirs.lst

$(SUBDIRS-y):
	@$(MAKE) -C $@ $(MAKEOP)
	@printf "  [D]\t$@\n"

$(SUBDIRS-n):
	@$(MAKE) -C $@ $(MAKEOP)

all: $(SUBDIRS-y) $(OBJS-y)

%.o:%.S
	@$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $(OBJ_DIR)/$(BUILD)/$@ $<
	@printf "  [M]\t$@\n"

%.o:%.c
	@$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -o $(OBJ_DIR)/$(BUILD)/$@ $<
	@printf "  [M]\t$@\n"

clean:
	@$(RM) $(OBJS-y) *.d
