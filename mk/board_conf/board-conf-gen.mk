include $(MKGEN_DIR)/build.mk
include $(ROOT_DIR)/mk/variables.mk

HOSTCC := cc

BOARD_CONF         := $(CONF_DIR)/board.conf.h
BOARD_CONF_DIR     := $(ROOT_DIR)/board_config
BOARD_CONF_GEN_DIR := $(ROOT_DIR)/mk/board_conf
BOARD_CONF_EXT_DIR := $(EXT_PROJECT_DIR)/board_config


board_config_h := $(SRCGEN_DIR)/include/config/board_config.h
GEN_EXE        := $(BUILD_DIR)/gen_board_conf

BOARD_CONF_INCLUDE := -I $(CONF_DIR) -I $(BOARD_CONF_DIR) -I $(BOARD_CONF_GEN_DIR)

ifneq ($(wildcard $(BOARD_CONF_EXT_DIR)),)
BOARD_CONF_INCLUDE += -I $(BOARD_CONF_EXT_DIR)
endif

ifneq ($(CHIP_VENDOR),)
BOARD_CONF_INCLUDE += -I $(CHIP_VENDOR_DIR)
endif

ifneq ($(PLATFORM_VENDOR),)
BOARD_CONF_INCLUDE += -I $(PLATFORM_VENDOR_DIR)
endif

all:
ifneq ($(wildcard $(BOARD_CONF)),)
	@echo ' BOARD_CONF $(BOARD_CONF_INCLUDE)'
	$(HOSTCC) $(BOARD_CONF_INCLUDE) -o $(GEN_EXE) $(BOARD_CONF_GEN_DIR)/gen_board_conf.c
	$(GEN_EXE) > $(board_config_h)
endif
