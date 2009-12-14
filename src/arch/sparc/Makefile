OBJS-y += sparc_traps.o
OBJS-y += apb_irq_ctrl.o
OBJS-y += apb_timers.o
OBJS-y += boot.o
OBJS-y += cpu_context.o
OBJS-y += entry.o
OBJS-y += trap_setup.o
OBJS-y += windows.o
OBJS-y += bubble_sort.o
OBJS-y += sys.o
OBJS-y += soft_trap_test.o

-include $(SELFDIR)/embox.lds.d

$(SELFDIR)/embox.lds:$(SELFDIR)/embox.lds.S $(BUILD_DIR)/config.lds.h
	$(CC) -I$(INCLUDE_DIR) -imacros $(BUILD_DIR)/config.lds.h -nostdinc \
		-MMD -MT $@ -MF $@.d -E -Wp, -P -Usparc -o $@ $<

clean: sparc_clean

# Force SELFDIR to be immediately expanded
sparc_dir:=$(SELFDIR)

sparc_clean:
	@$(RM) $(sparc_dir)/embox.lds @$(RM) $(sparc_dir)/embox.lds.d

include $(SCRIPTS_DIR)/common.mk
