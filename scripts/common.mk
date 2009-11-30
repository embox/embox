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
	@printf "  [M]\t$@\n"
	@$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -I$(INCLUDE_DIR) -o $@ $<

%.o:%.c
	@printf "  [M]\t$@\n"
	@$(CC) $(CCFLAGS) $(INCLUDE_DIRS) -I$(INCLUDE_DIR) -o $@ $<

copy_objs:
	@if [ ! -z "$(OBJS-y)" ]; then \
	    for OBJ in $(OBJS-y); do \
		    cp -f $$OBJ $(OBJ_DIR)/$(BUILD)/$(shell pwd | openssl dgst -md5)-$$OBJ; \
	    done; \
	fi

clean: $(SUBDIRS-y)
	@printf "  [R]\t$(SUBDIRS-y) $(OBJS-y)\n"
	@rm -f  $(OBJS-y)
