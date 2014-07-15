
TOOLCHAIN_TEST_SRC := $(ROOT_DIR)/mk/extbld/toolchain_test.c
TOOLCHAIN_TEST_OUT := $(OBJ_DIR)/toolchain_test

include $(EXTBLD_LIB)
include $(SRCGEN_DIR)/build.mk

.PHONY : do_test
do_test :
ifeq ($(filter usermode%,$(ARCH)),)
	EMBOX_GCC_LINK=full $(EMBOX_GCC) $(TOOLCHAIN_TEST_SRC) -o $(TOOLCHAIN_TEST_OUT)
else
	@echo "Full linking mode isn't supported for usermode arch!"
endif

