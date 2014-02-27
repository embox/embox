ifeq ($(TARGET_APP),)
$(error  is not set.)
endif

ifeq ($(FILE_APP),)
$(error  is not set.)
endif


OBJCOPY := $(EMBOX_CROSS_COMPILE)objcopy
CC := $(EMBOX_CROSS_COMPILE)gcc
LD := $(EMBOX_CROSS_COMPILE)ld

$(FILE_APP): $(subst strip.,,$(FILE_APP))
	echo "stripping"
	$(OBJCOPY) --redefine-sym=main=main_$(TARGET_APP) $< $<.redef
	$(CC) -DMAIN_ROUTING_NAME=main_$(TARGET_APP) \
		-D__EMBUILD_MOD__=$(TARGET_APP) \
		$(EMBOX_CFLAGS) \
		$(EMBOX_CPPFLAGS) \
		-c -o $<.cmd $(abspath $(ROOT_DIR))/mk/script/application_template.c
	$(LD) -r $(EMBOX_LDFLAGS) $<.redef $<.cmd -o $@
