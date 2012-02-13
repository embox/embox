
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

endif

# Expand user defined flags and append them after default ones.

# Preprocessor flags
cppflags := $(CPPFLAGS)
override CPPFLAGS  = -D__EMBOX__
override CPPFLAGS += -D"__module_headers(path)=<$(patsubst \
          $(abspath $(ROOT_DIR))/%,../../%,$(abspath $(OBJ_DIR))/mods/path.h)>"
override CPPFLAGS += -D"__impl_x(path)=<../path>"
override CPPFLAGS += -imacros $(AUTOCONF_DIR)/config.h
override CPPFLAGS += -I$(SRC_DIR)/include -I$(SRC_DIR)/arch/$(ARCH)/include
# XXX reduntand flags, agrrrr -- Eldar
override CPPFLAGS += -I$(SRC_DIR)/compat/linux/include -I$(SRC_DIR)/compat/posix/include
override CPPFLAGS += -nostdinc
override CPPFLAGS += -MMD -MP# -MT $@ -MF $(@:.o=.d)
override CPPFLAGS += $(cppflags)

# Assembler flags
asflags := $(CFLAGS)
override ASFLAGS  = -pipe
override ASFLAGS += $(asflags)

# Compiler flags
cflags := $(CFLAGS)
override CFLAGS  = -std=gnu99
override CFLAGS += -fno-strict-aliasing -fno-common
override CFLAGS += -Wall -Werror
override CFLAGS += -Wstrict-prototypes -Wdeclaration-after-statement
override CFLAGS += -Wundef -Wno-trigraphs -Wno-char-subscripts
override CFLAGS += -Wformat -Wformat-nonliteral -Wno-format-zero-length
override CFLAGS += -pipe
override CFLAGS += $(cflags)

# Linker flags
ldflags := $(LDFLAGS)
override LDFLAGS  = -static
override LDFLAGS += -nostdlib
override LDFLAGS += --cref --relax
#override LDFLAGS += $(SUBDIRS_LDFLAGS)
override LDFLAGS += $(ldflags)

override ARFLAGS = rcs

CCFLAGS ?=

