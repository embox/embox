ifndef mk_image_lib_
mk_image_lib_ := 1

include mk/core/common.mk

ifndef LD_SINGLE_T_OPTION
ld_scripts_flag = $(1:%=-T%)
else
ld_scripts_flag = $(if $(strip $1),-T $1)
endif

# This must be expanded in a secondary expansion context.
# NOTE: must be the last one in a list of prerequisites (contains order-only)
common_prereqs = mk/image2.mk mk/image3.mk mk/image_lib.mk mk/flags.mk \
	$(MKGEN_DIR)/build.mk \
	$(if $(value mk_file),$(mk_file)) \
	| $(if $(value my_file),$(dir $(my_file:%=$(OBJ_DIR)/%)).) $(@D)/.


VPATH := $(SRCGEN_DIR)

%/. :
	@$(MKDIR) $*

a_prerequisites     = $(common_prereqs)
o_prerequisites     = $(common_prereqs)
cc_prerequisites    = $(common_prereqs)
cpp_prerequisites   = $(common_prereqs)
extbld_prerequisites= $(common_prereqs)

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.c
	$(CC) $(flags_before) $(CFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.S
	$(CC) $(flags_before) $(ASFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.cpp
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<
$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.cxx
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<
$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.C
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.lds : $(ROOT_DIR)/%.lds.S
	$(CPP) $(flags_before) -P -undef -D__LDS__ $(CPPFLAGS) $(flags) \
	-imacros $(SRCGEN_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -o $@ $<

endif
