
CFLAGS ?=
CPPFLAGS ?=
ASFLAGS ?=
ARFLAGS ?=
LDFLAGS ?=

ifneq ($(patsubst N,0,$(patsubst n,0,$(or $(value NDEBUG),0))),0)
override CPPFLAGS += -DNDEBUG
override NDEBUG := 1
else
override NDEBUG :=
endif

ifdef OPTIMIZE

override OPTIMIZE := $(strip $(OPTIMIZE:-O%=%))
__optimize_valid_values := s 0 1 2 3 4 5 99
__optimize_invalid := $(filter-out $(__optimize_valid_values),$(OPTIMIZE))
ifneq ($(__optimize_invalid),)
$(error Invalid value for OPTIMIZE flag: $(__optimize_invalid). \
  Valid values are: $(__optimize_valid_values))
endif

ifneq ($(words $(OPTIMIZE)),1)
$(error Only single value for OPTIMIZE flag is permitted)
endif

override CFLAGS += -O$(OPTIMIZE)
override CXXFLAGS += -O$(OPTIMIZE)

endif

# Expand user defined flags and append them after default ones.

# Preprocessor flags
cppflags := $(CPPFLAGS)
override CPPFLAGS  = -D__EMBOX__
override CPPFLAGS += -D"__impl_x(path)=<../path>"
override CPPFLAGS += -imacros $(AUTOCONF_DIR)/config.h
override CPPFLAGS += -I$(SRC_DIR)/include -I$(SRC_DIR)/arch/$(ARCH)/include
override CPPFLAGS += -I$(SRCGEN_DIR)/include -I$(SRCGEN_DIR)/src/include
__srcgen_includes := $(addprefix $(SRCGEN_DIR)/src/,include arch/$(ARCH)/include)
$(and $(shell $(MKDIR) $(__srcgen_includes)),)
override CPPFLAGS += $(__srcgen_includes:%=-I%)
# XXX reduntand flags, agrrrr -- Eldar
override CPPFLAGS += $(if $(value PLATFORM),-I$(PLATFORM_DIR)/$(PLATFORM)/include)
override CPPFLAGS += -I$(SRC_DIR)/compat/linux/include -I$(SRC_DIR)/compat/posix/include
override CPPFLAGS += -nostdinc
override CPPFLAGS += -MMD -MP# -MT $@ -MF $(@:.o=.d)
override CPPFLAGS += $(cppflags)

cppflags_fn = \
	-D__EMBOX__ \
	-D"__impl_x(path)=<../path>"\
	-imacros $(AUTOCONF_DIR)/config.h\
	-I$1$(SRC_DIR)/include -I$1$(SRC_DIR)/arch/$(ARCH)/include\
	-I$1$(SRCGEN_DIR)/include -I$1$(SRCGEN_DIR)/src/include\
	$(addprefix $1$(SRCGEN_DIR)/src/,include arch/$(ARCH)/include)\
	$(__srcgen_includes:%=-I%)\
	$(if $(value PLATFORM),-I$1$(PLATFORM_DIR)/$(PLATFORM)/include)\
	-I$1$(SRC_DIR)/compat/linux/include -I$1$(SRC_DIR)/compat/posix/include\
	-nostdinc\
	-MMD -MP# -MT $@ -MF $(@:.o=.d)\
	$(cppflags)

EMBOX_EXPORT_CPPFLAGS := $(call cppflags_fn,$(PWD))

# Assembler flags
asflags := $(CFLAGS)
override ASFLAGS  = -pipe
override ASFLAGS += $(asflags)


cxxflags := $(CFLAGS)
override CXXFLAGS = -pipe
override CXXFLAGS += -fno-strict-aliasing -fno-common
override CXXFLAGS += -Wall -Werror
override CXXFLAGS += -Wundef -Wno-trigraphs -Wno-char-subscripts
override CXXFLAGS += -Wformat -Wformat-nonliteral
override CXXFLAGS += -I$(SRC_DIR)/include/c++
override CXXFLAGS += -D"__BEGIN_DECLS=extern \"C\" {"
override CXXFLAGS += -D"__END_DECLS=}"
#	C++ has build-in type bool
override CXXFLAGS += -DSTDBOOL_H_
override CXXFLAGS += $(cxxflags)

# Compiler flags
cflags := $(CFLAGS)
override CFLAGS  = -std=gnu99
override CFLAGS += -fno-strict-aliasing -fno-common
override CFLAGS += -Wall -Werror
override CFLAGS += -Wstrict-prototypes -Wdeclaration-after-statement
override CFLAGS += -Wundef -Wno-trigraphs -Wno-char-subscripts
override CFLAGS += -Wformat -Wno-format-zero-length #-Wformat-nonliteral
override CFLAGS += -pipe
override CFLAGS += -D"__BEGIN_DECLS="
override CFLAGS += -D"__END_DECLS="
override CFLAGS += $(cflags)

# Linker flags
ldflags := $(LDFLAGS)
override LDFLAGS  = -static -nostdlib
override LDFLAGS += $(ldflags)

override ARFLAGS = rcs



CCFLAGS ?=

