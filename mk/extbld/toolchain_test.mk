
TOOLCHAIN_TEST_SRC := $(ROOT_DIR)/mk/extbld/toolchain_test.c
TOOLCHAIN_TEST_OUT := $(OBJ_DIR)/toolchain_test

include $(EXTBLD_LIB)

.PHONY : do_test
do_test :
	EMBOX_GCC_LINK=full $(EMBOX_GCC) $(TOOLCHAIN_TEST_SRC) -o $(TOOLCHAIN_TEST_OUT)
