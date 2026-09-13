
PKG_NAME := examples

DRONECAN_EXT_DIR := $(EXTERNAL_BUILD_DIR)/project/dronecan

DSDL_DIR := $(DRONECAN_EXT_DIR)/dsdl/install
DSDLC_DIR := $(DRONECAN_EXT_DIR)/dronecan_dsdlc/install
DSDLC_PYTHON := $(DSDLC_DIR)/venv/bin/python
DSDLC_SCRIPT := $(DSDLC_DIR)/dronecan_dsdlc.py

LIBCANARD_DIR := $(wildcard $(DRONECAN_EXT_DIR)/libcanard/libcanard-*)
SIMPLE_NODE_SRC := $(LIBCANARD_DIR)/examples/SimpleNode/simple_node.c
SIMPLE_NODE_DST := $(PKG_INSTALL_DIR)/src/simple_node.c

DSDL_ARGS := \
	uavcan.protocol.NodeStatus \
	uavcan.protocol.GetNodeInfo

DSDLC_BUILD_ARGS := $(addprefix --build ,$(DSDL_ARGS))

$(CONFIGURE):
	$(MKDIR) $(PKG_INSTALL_DIR)

	$(DSDLC_PYTHON) $(DSDLC_SCRIPT)\
		-O $(PKG_INSTALL_DIR) \
		$(DSDLC_BUILD_ARGS) \
		$(DSDL_DIR)/dronecan \
		$(DSDL_DIR)/uavcan \
		$(DSDL_DIR)/com \
		$(DSDL_DIR)/ardupilot

	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(MKDIR) $(dir $(SIMPLE_NODE_DST))
	$(CP) $(SIMPLE_NODE_SRC) $(SIMPLE_NODE_DST)

	sed -i \
		-e 's|#include <socketcan.h>|#include <dronecan/canard_embox.h>|' \
		-e 's/SocketCANInstance/CanardEmboxInstance/g' \
		-e 's/socketcanTransmit/canardEmboxTransmit/g' \
		-e 's/socketcanReceive/canardEmboxReceive/g' \
		-e 's/socketcanInit/canardEmboxInit/g' \
		$(SIMPLE_NODE_DST)

	touch $@
