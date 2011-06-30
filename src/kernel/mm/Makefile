$_PACKAGE := embox.hal.mm

$_MODS += mmu mpallocator opallocator slab

ifdef CONFIG_HEAP_SIZE
# describe special section
$_heap_lds := $(OBJ_DIR)/kernel/mm/heap.lds
$_LDFLAGS += $($_heap_lds)

$(IMAGE): $($_heap_lds)

$($_heap_lds): $($_SELFDIR)/heap.lds.S $(AUTOCONF_DIR)/config.lds.h
	mkdir -p $(@D) && $(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $($_heap_lds).d
endif
