ifeq ($(TARGET_APP),)
$(error TARGET_APP is not set.)
endif

ifeq ($(FILE_APP),)
$(error FILE_APP is not set.)
endif

MODULE_ID :=$(TARGET_APP)
TARGET_OBJ:=$(FILE_APP)
SOURCE_OBJ:=$(subst strip.,,$(TARGET_OBJ))

OBJCOPY := $(EMBOX_CROSS_COMPILE)objcopy
OBJDUMP := $(EMBOX_CROSS_COMPILE)objdump
CC := $(EMBOX_CROSS_COMPILE)gcc
LD := $(EMBOX_CROSS_COMPILE)ld

CMD_WRAPPER_SRC:=$(abspath $(ROOT_DIR))/mk/script/application_template.c

redefd_main_obj=$<.redef.o
cmd_wrapper_obj=$<.cmd.o
localize_symbols=$<.lsyms.txt
main_rename_name=main_$(MODULE_ID)

$(TARGET_OBJ): $(SOURCE_OBJ)
	$(OBJDUMP) -t $< | grep " g " | tr -s ' ' | cut -d \  -f 5 | \
		grep -v main > $(localize_symbols)
	$(OBJCOPY) --localize-symbols=$(localize_symbols) \
		--redefine-sym=main=$(main_rename_name) \
		$< $(redefd_main_obj)
	$(CC) -DMAIN_ROUTING_NAME=$(main_rename_name) \
		-D__EMBUILD_MOD__=$(MODULE_ID) \
		$(EMBOX_CFLAGS) \
		$(EMBOX_CPPFLAGS) \
		-c -o $(cmd_wrapper_obj) $(CMD_WRAPPER_SRC)
	$(LD) -r $(EMBOX_LDFLAGS) $(redefd_main_obj) $(cmd_wrapper_obj) -o $@
