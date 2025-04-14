##
# Definitions used by user Makefile
#

# Params:
#  1. Option type
#  2. Option name
# Return:
#  Option value
define option_get
$(shell echo OPTION_GET\($(1),$(2)\) | $(EMBOX_GCC) $(EMBOX_MODULE_CPPFLAGS) \
	-include $(SRC_DIR)/include/framework/mod/options.h -P -E -)
endef

# Params:
#  1. GCC macro
# Return:
#  Macro value if defined or empty string
define gcc_macro_get
$(subst $(1),,$(shell echo $(1) | $(EMBOX_GCC) -P -E -))
endef

# Params:
#  1. GXX macro
# Return:
#  Macro value if defined or empty string
define gxx_macro_get
$(subst $(1),,$(shell echo $(1) | $(EMBOX_GXX) -P -E -))
endef
