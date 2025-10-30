
include mk/flags.mk

TOOLCHAIN_TEST_SRC := $(ROOT_DIR)/mk/extbld/toolchain_test.c
TOOLCHAIN_TEST_OUT := $(OBJ_DIR)/toolchain_test

.PHONY : all
all : $(TOOLCHAIN_TEST_OUT)

$(TOOLCHAIN_TEST_OUT):
ifeq ($(COMPILER),clang)
	@touch $@
else ifeq ($(filter usermode%,$(ARCH)),)
	EMBOX_GCC_LINK=full EMBOX_CROSS_COMPILE=$(CROSS_COMPILE) \
		$(EMBOX_GCC) $(TOOLCHAIN_TEST_SRC) -o $(TOOLCHAIN_TEST_OUT)
else
	@echo "Full linking mode isn't supported for usermode arch!"
endif
