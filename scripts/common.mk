.PHONY: all $(SUBDIRS-y) $(SUBDIRS-n)

$(SUBDIRS-y):
	@$(MAKE) -C $@ $(MAKEOP)

$(SUBDIRS-n):
	@$(MAKE) -C $@ $(MAKEOP)

all: $(SUBDIRS-y) $(OBJS-y)

%.o:%.S
	@printf "  [M]\t$(subst $(SRC_DIR)/,,$(abspath $@))\n"
	@$(CC) $(CCFLAGS) -I$(INCLUDE_DIR) -o $@ $<
#	@cp -f $@ $(OBJ_DIR)/$(BUILD)/$(subst /,__,$(subst $(SRC_DIR),,$(abspath $@)))
	@ln -s $(abspath $@) $(OBJ_DIR)/$(BUILD)/$(subst /,__,$(subst $(SRC_DIR),,$(abspath $@)))

%.o:%.c
	@printf "  [M]\t$(subst $(SRC_DIR)/,,$(abspath $@))\n"
	@$(CC) $(CCFLAGS) -I$(INCLUDE_DIR) -o $@ $<
#	@cp -f $@ $(OBJ_DIR)/$(BUILD)/$(subst /,__,$(subst $(SRC_DIR),,$(abspath $@)))
	@ln -s $(abspath $@) $(OBJ_DIR)/$(BUILD)/$(subst /,__,$(subst $(SRC_DIR),,$(abspath $@)))

#copy_objs:
#	@if [ ! -z "$(OBJS-y)" ]; then \
#	    for OBJ in $(OBJS-y); do \
#		    cp -f $$OBJ $(OBJ_DIR)/$(BUILD)/$(shell pwd | openssl dgst -md5)-$$OBJ; \
#	    done; \
#	fi

clean: $(SUBDIRS-y)
	@rm -fv *.o *.d
