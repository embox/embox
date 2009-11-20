.PHONY: all $(SUBDIRS-y) $(SUBDIRS-n) copy_objs

create_objs_lst: $(SUBDIRS-y)
	@echo '$(addprefix $(CURDIR)/,$(OBJS-y)) \' >> $(ROOT_DIR)/objs.lst

create_include_dirs_lst: $(SUBDIRS-y) $(SUBDIRS-n)
	@echo ' -I$(CURDIR)\' >> $(ROOT_DIR)/include_dirs.lst

$(SUBDIRS-y):
	@$(MAKE) -C $@ $(MAKEOP)

$(SUBDIRS-n):
	@$(MAKE) -C $@ $(MAKEOP)

all: $(SUBDIRS-y) $(OBJS-y) copy_objs

%.o:%.S
	@$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -I$(INCLUDE_DIR) -o $@ $<
	@printf "  [M]\t$@\n"

%.o:%.c
	@$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -I$(INCLUDE_DIR) -o $@ $<
	@printf "  [M]\t$@\n"

copy_objs:
	@if [ ! -z "$(OBJS-y)" ]; then \
	    for OBJ in $(OBJS-y); do \
		    cp -f $$OBJ $(OBJ_DIR)/$(BUILD)/`date +%N`-$$OBJ; \
	    done; \
	fi

clean: $(SUBDIRS-y)
	@rm -f  $(OBJS-y)
	@printf "  [R]\t$(SUBDIRS-y) $(OBJS-y)\n"
