
L_CFLAGS = -nostdlib -emain -fpie -N

$(LOADABLE_DIR)/$(DST) : | $(LOADABLE_DIR)
$(LOADABLE_DIR)/$(DST) : $(SRC)
	$(EMBOX_CROSS_COMPILE)gcc $(L_CFLAGS) -o $@ $<

$(LOADABLE_DIR) :
	mkdir -p $@
