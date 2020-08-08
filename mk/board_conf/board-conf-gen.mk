
HOSTCC  = cc

BOARD_CONF_DIR := board_config/
BOARD_CONF := $(CONF_DIR)/board.conf.h
BOARD_CONF_GEN_DIR := $(ROOT_DIR)/mk/board_conf
board_config_h := $(SRCGEN_DIR)/include/config/board_config.h
GEN_EXE := $(BUILD_DIR)/gen_board_conf

all:
ifneq ($(wildcard $(BOARD_CONF)),)
	$(HOSTCC) -I $(CONF_DIR) -I $(BOARD_CONF_DIR) -I $(BOARD_CONF_GEN_DIR) \
		-o $(GEN_EXE) $(BOARD_CONF_GEN_DIR)/gen_board_conf.c
	$(GEN_EXE) > $(board_config_h)
endif
