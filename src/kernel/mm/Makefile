$_PACKAGE := embox.hal.mm

$_MODS += mmu mpallocator opallocator kmalloc

$_SUBDIRS := $(filter-out _template,$($_SUBDIRS))

ifdef CONFIG_HEAP_SIZE
#describe spetial section
$_LDFLAGS += $($_heap_lds)

$_heap_lds := $(OBJ_DIR)/kernel/mm/heap.lds
$(IMAGE): $($_heap_lds)

$($_heap_lds): $($_SELFDIR)/heap.lds.S $(AUTOCONF_DIR)/config.lds.h
	mkdir -p $(@D) && $(CPP) -Wp, -P -undef $(CPPFLAGS) \
		-imacros $(AUTOCONF_DIR)/config.lds.h -MMD -MT $@ -MF $@.d -o $@ $<

-include $($_piggy_lds).d
endif
