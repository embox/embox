OBJS-y += interrupt_handler.o
OBJS-y += mb_irq_ctrl.o
OBJS-y += mb_timers.o
OBJS-y += entry.o
OBJS-y += boot.o
OBJS-y += sys.o

-include $(SELFDIR)/embox.lds.d

$(SELFDIR)/embox.lds:$(SELFDIR)/embox.lds.S $(BUILD_DIR)/config.lds.h
	$(CC) $(CCINCLUDES) -imacros $(BUILD_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -E -Wp, -P -undef -o $@ $<

clean: arch_clean

# Force SELFDIR to be immediately expanded
arch_dir:=$(SELFDIR)

arch_clean:
	@$(RM) $(arch_dir)/embox.lds @$(RM) $(arch_dir)/embox.lds.d

include $(SCRIPTS_DIR)/common.mk

