
PKG_NAME    := libcxx
PKG_VER     :=
PKG_SOURCES := https://github.com/llvm-mirror/libcxx/archive/refs/heads/master.zip
PKG_MD5     := 2fca36f434b24201356ef2329617b862
PKG_PATCHES := disable_wchar.patch

LIBCXX_SRCDIR := $(MOD_BUILD_DIR)/libcxx-master/src
LIBCXX_INCDIR := $(MOD_BUILD_DIR)/libcxx-master/include

LIBCXX_SRCS := \
	$(LIBCXX_SRCDIR)/algorithm.cpp \
	$(LIBCXX_SRCDIR)/any.cpp \
	$(LIBCXX_SRCDIR)/bind.cpp \
	$(LIBCXX_SRCDIR)/charconv.cpp \
	$(LIBCXX_SRCDIR)/chrono.cpp \
	$(LIBCXX_SRCDIR)/condition_variable.cpp \
	$(LIBCXX_SRCDIR)/condition_variable_destructor.cpp \
	$(LIBCXX_SRCDIR)/debug.cpp \
	$(LIBCXX_SRCDIR)/exception.cpp \
	$(LIBCXX_SRCDIR)/functional.cpp \
	$(LIBCXX_SRCDIR)/future.cpp \
	$(LIBCXX_SRCDIR)/hash.cpp \
	$(LIBCXX_SRCDIR)/ios.cpp \
	$(LIBCXX_SRCDIR)/iostream.cpp \
	$(LIBCXX_SRCDIR)/locale.cpp \
	$(LIBCXX_SRCDIR)/memory.cpp \
	$(LIBCXX_SRCDIR)/mutex.cpp \
	$(LIBCXX_SRCDIR)/mutex_destructor.cpp \
	$(LIBCXX_SRCDIR)/new.cpp \
	$(LIBCXX_SRCDIR)/optional.cpp \
	$(LIBCXX_SRCDIR)/random.cpp \
	$(LIBCXX_SRCDIR)/regex.cpp \
	$(LIBCXX_SRCDIR)/shared_mutex.cpp \
	$(LIBCXX_SRCDIR)/stdexcept.cpp \
	$(LIBCXX_SRCDIR)/string.cpp \
	$(LIBCXX_SRCDIR)/strstream.cpp \
	$(LIBCXX_SRCDIR)/system_error.cpp \
	$(LIBCXX_SRCDIR)/thread.cpp \
	$(LIBCXX_SRCDIR)/typeinfo.cpp \
	$(LIBCXX_SRCDIR)/utility.cpp \
	$(LIBCXX_SRCDIR)/valarray.cpp \
	$(LIBCXX_SRCDIR)/variant.cpp \
	$(LIBCXX_SRCDIR)/vector.cpp

LIBCXX_FLAGS := \
	-ffunction-sections \
	-fdata-sections \
	-fdevirtualize \
	-fvisibility-inlines-hidden \
	-fvisibility=hidden \
	-D_LIBCPP_BUILDING_LIBRARY \
	-DLIBCXX_BUILDING_LIBCXXABI \
	-I$(LIBCXX_INCDIR)

ifeq (1,$(call option_get,BOOLEAN,optimize_size))
LIBCXX_FLAGS += -Os
endif

ifeq (1,$(call option_get,BOOLEAN,enable_assertions))
LIBCXX_FLAGS += -D_LIBCPP_DEBUG=1
endif

LIBCXX_OBJS := $(patsubst %.cpp,%.o,$(LIBCXX_SRCS))
$(LIBCXX_OBJS) : $(CONFIGURE)

LIBCXX_A := $(PKG_INSTALL_DIR)/libcxx.a
$(LIBCXX_A) : $(LIBCXX_OBJS)
	$(TOOLCHAIN_DIR)/embox-ar rcs $@ $^

$(LIBCXX_SRCDIR)/%.o : $(LIBCXX_SRCDIR)/%.cpp
	$(EMBOX_CLANGXX) $(LIBCXX_FLAGS) -c -o $@ $<

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR)/include && ( \
		$(MV) __config __config_old; \
		cat $(CURDIR)/__config __config_old > __config; \
	)
	touch $@

$(BUILD) : $(LIBCXX_A)
	touch $@

$(INSTALL) :
	touch $@
