ifndef __image_lib_mk
__image_lib_mk := 1

include mk/core/common.mk

# This must be expanded in a secondary expansion context.
# NOTE: must be the last one in a list of prerequisites (contains order-only)
common_prereqs = mk/image2.mk mk/image3.mk mk/image_lib.mk mk/flags.mk \
	$(MKGEN_DIR)/build.mk \
	$(if $(value mk_file),$(mk_file)) \
	| $(if $(value my_file),$(dir $(my_file:%=$(OBJ_DIR)/%)).) $(@D)/.


VPATH := $(SRCGEN_DIR)

%/. :
	@$(MKDIR) $*

a_prerequisites               = $(common_prereqs)
o_prerequisites               = $(common_prereqs)
cc_prerequisites              = $(common_prereqs)
cpp_prerequisites             = $(common_prereqs)
extbld_prerequisites          = $(common_prereqs)
include_install_prerequisites = $(common_prereqs)

flags_before ?=
flags ?=
iec2c ?=
ieclib ?=

ifndef GEN_DIST
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

$(OBJ_DIR)/%.o : $(ROOT_DIR)/%.cc
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.lds : $(ROOT_DIR)/%.lds.S
	$(CPP) $(flags_before) $(CFLAGS) -P -D__LDS__ $(CPPFLAGS) $(flags) \
	-I$(SRCGEN_DIR) \
	-imacros $(SRCGEN_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -o $@ $<

$(GEN_DIR)/%.st.c : $(ROOT_DIR)/%.st
	IEC2C=$(iec2c) IECLIB=$(ieclib) $(ROOT_DIR)/mk/plc/iec2c.sh $< $@
endif # GEN_DIST

$(OBJ_DIR)/%.o : $(GEN_DIR)/%.c
	$(CC) $(flags_before) $(CFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(GEN_DIR)/%.S
	$(CC) $(flags_before) $(ASFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(GEN_DIR)/%.cpp
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(GEN_DIR)/%.cxx
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(GEN_DIR)/%.C
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.o : $(GEN_DIR)/%.cc
	$(CXX) $(flags_before) $(CXXFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<

$(OBJ_DIR)/%.lds : $(GEN_DIR)/%.lds.S
	$(CPP) $(flags_before) $(CFLAGS) -P -D__LDS__ $(CPPFLAGS) $(flags) \
	-I$(SRCGEN_DIR) \
	-imacros $(SRCGEN_DIR)/config.lds.h \
		-MMD -MT $@ -MF $@.d -o $@ $<

$(GEN_DIR)/%.st.c : $(GEN_DIR)/%.st
	IEC2C=$(iec2c) IECLIB=$(ieclib) $(ROOT_DIR)/mk/plc/iec2c.sh $< $@

$(OBJ_DIR)/%.o : $(GEN_DIR)/%.st.c
	$(CC) $(flags_before) $(CFLAGS) $(CPPFLAGS) $(flags) -c -o $@ $<
	$(ROOT_DIR)/mk/plc/localize_symbols.sh $@

ifeq ($(value OSTYPE),cygwin)
# GCC built for Windows doesn't recognize /cygdrive/... absolute paths. As a
# workaround, for every rule calling GCC (determined through the target
# extension: .o and .lds) we invoke a sub-shell, passing a patched command to
# it. That is,
#
#     sh -c 'gcc ... /cygdrive/path/to/embox/src/file.c'
#
# Becomes:
#
#     sh -c "${0//$PWD/.}" "sh -c 'gcc ... /cygdrive/path/to/embox/src/file.c'"
#
# Which, in turn, expands by the outer shell to (roughly) the following:
#
#     sh -c "sh -c 'gcc ... ./src/file.c'"
#
# This is a _really_ dirty hack. It also breaks the build in case of using dash
# as a shell, since the latter doesn't understand ${PARAMETER//PATTERN/STRING}
# expansions, resulting in a 'Bad substitution' error.
.SHELLFLAGS = \
	-c$(if $(filter %.o %.lds,$(value @)), \
		'$(SHELL) -c "$${0//$$PWD/.}"')
endif

endif # __image_lib_mk
