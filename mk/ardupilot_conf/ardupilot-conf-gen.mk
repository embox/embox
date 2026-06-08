
include mk/flags.mk

ARDUPILOT_CONF      := $(CONF_DIR)/ardupilot_conf.h

all:
ifneq ($(wildcard $(ARDUPILOT_CONF)),)
	$(CP) $(ARDUPILOT_CONF) $(INCLUDE_INSTALL_DIR)/ardupilot_conf.h
else
	touch $(INCLUDE_INSTALL_DIR)/ardupilot_conf.h
endif
