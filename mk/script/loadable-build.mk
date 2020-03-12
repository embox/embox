
L_CFLAGS = -nostdlib -emain -fpie -static

$(LOADABLE_DIR)/$(DST) : | $(LOADABLE_DIR)
$(LOADABLE_DIR)/$(DST) : $(SRC)
	$(EMBOX_CROSS_COMPILE)gcc $(EMBOX_CFLAGS) $(L_CFLAGS) -o $@ $<

$(LOADABLE_DIR) :
	mkdir -p $@
