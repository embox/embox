
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

AT=

STLPORT_DIR=$(ROOT_DIR)/build/stlport
STLPORT_CMAKE_DIR=$(STLPORT_DIR)/cmake
STLPORT_LIB=$(ROOT_DIR)/lib/libstlportg.a

all:

.PHONY: all

$(STLPORT_CMAKE_DIR):
	$(AT)mkdir -p $@

$(STLPORT_CMAKE_DIR)/Makefile: CMakeLists.txt | $(STLPORT_CMAKE_DIR)
	$(AT)cd $(STLPORT_CMAKE_DIR) && cmake $(shell pwd)

$(STLPORT_LIB): $(STLPORT_CMAKE_DIR)/Makefile
	$(AT)cd $(STLPORT_CMAKE_DIR) && $(MAKE) # -j `nproc`

all: $(STLPORT_LIB)

