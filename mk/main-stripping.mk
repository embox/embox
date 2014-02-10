ifeq ($(TARGET_APP),)
$(error  is not set.)
endif

ifeq ($(FILE_APP),)
$(error  is not set.)
endif


OBJCOPY := $(EMBOX_CROSS_COMPILE)objcopy
CC := $(EMBOX_CROSS_COMPILE)gcc
LD := $(EMBOX_CROSS_COMPILE)ld

all: $(FILE_APP).comand_section
	echo "stripping"
	$(LD) -r $(EMBOX_LDFLAGS) $(FILE_APP) $(FILE_APP).comand_section -o $(FILE_APP).merged
	cp $(FILE_APP) $(FILE_APP).pure
	cp $(FILE_APP).merged $(FILE_APP)

$(FILE_APP).comand_section:
	$(OBJCOPY) --redefine-sym=main=main_$(TARGET_APP) $(FILE_APP)
	$(CC) -DMAIN_ROUTING_NAME=main_$(TARGET_APP) -D__EMBUILD_MOD__=$(TARGET_APP) $(EMBOX_CFLAGS) $(EMBOX_CPPFLAGS) -c -o $@  $(abspath $(ROOT_DIR))/mk/script/application_template.c

