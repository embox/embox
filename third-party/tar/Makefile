DOWNLOAD_DIR   := $(ROOT_DIR)/download

PKG_DIR         := $(BUILD_DIR)
PKG_SRC_DIR     := $(PKG_DIR)/source
PKG_BUILD_DIR   := $(PKG_DIR)/build
PKG_INSTALL_DIR := $(PKG_DIR)/install

DOWNLOADED  := $(PKG_DIR)/.downloaded
EXTRACTED   := $(PKG_DIR)/.extracted
CONFIGURED  := $(PKG_DIR)/.configured
ENVIRONMENT := $(PKG_DIR)/.environment

PKG_URL    := http://ftp.gnu.org/gnu/tar/tar-1.27.1.tar.gz

PKG_BUNDLE := $(DOWNLOAD_DIR)/$(notdir $(PKG_URL))

PKG_PATCH := pkg_patch.txt

PKG_CONF_FLAGS:=--build=x86_64-linux-gnu --host=i386-embox-none
#PKG_CONF_FLAG +=--target=$(EMBOX_IMPORTED_CROSS_COMPILE)




EMBOX_IMPORTED_CPPFLAGS :=
EMBOX_IMPORTED_CPPFLAGS += -save-temps=obj
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS :=
EMBOX_IMPORTED_CFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -march%,$(EMBOX_CFLAGS))
#ARCH specific
EMBOX_IMPORTED_CFLAGS += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS :=
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(subst .,$(ROOT_DIR),$(filter -I%,$(EMBOX_CXXFLAGS))) # remove me
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
#arch specific
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_MAKEFLAGS :=
ifneq (,$(filter -j,$(EMBOX_MAKEFLAGS)))
EMBOX_IMPORTED_MAKEFLAGS += -j $(shell nproc)
endif

EMBOX_DERIVED_CFLAGS    := $(EMBOX_IMPORTED_CFLAGS)
EMBOX_DERIVED_CXXFLAGS  := $(EMBOX_IMPORTED_CXXFLAGS)
EMBOX_DERIVED_MAKEFLAGS := $(EMBOX_IMPORTED_MAKEFLAGS)

export CFLAGS    := $(EMBOX_IMPORTED_CFLAGS)
export CXXFLAGS  := $(EMBOX_IMPORTED_CXXFLAGS)
#export LDFLAGS  := $(EMBOX_IMPORTED_LDFLAGS)
export LDFLAGS   := -nostdlib

.PHONY: all build_and_install configure merge_trees

all: build_and_install

$(PKG_BUILD_DIR):
	mkdir -p $@

PKG_MD5_LINE:= $(PKG_BUNDLE)

$(DOWNLOADED):
	wget -P $(DOWNLOAD_DIR) -c $(PKG_URL)
	mkdir -p $(dir $@)
	touch $@

# When necessity to re-patch is detected manual resolution is required.
# We cannot automatically detect if there were or not changes in extracted sources
# and do not want to accidentally remove some possible desired changes.
# Therefore it is left to the discretion of the user.
# HINT: Typically it is good to perform build directory cleanup: 'make clean'
# The patch is saved to .extracted for the user to be able to compare previous patch with current one.
# In some situations this might be useful.
$(EXTRACTED): $(PKG_PATCH) $(DOWNLOADED)
	 [ ! -e $@ ] || (echo Re-patching is needed. Please resolve manually. && false)
	rm -rf $(PKG_SRC_DIR)
	mkdir -p $(PKG_SRC_DIR)
	tar -C $(PKG_SRC_DIR) -xzf $(abspath $(PKG_BUNDLE)) --strip-components=1
	patch -p1 -d $(PKG_SRC_DIR) -N --input=$(abspath $(PKG_PATCH))
	cp $(PKG_PATCH) $@

# using -f since 'svn update' causes unlink and 'cp -u' complaints
# http://svn.haxx.se/users/archive-2007-10/0797.shtml
# but typically overwriting is what we want for our part of tree contents
merge_trees: $(EXTRACTED)
	#cd tree && cp -l -u -f --parents -t $(PKG_SRC_DIR)/ `find . -not -path '*/.*' -and -type f`


.PHONY: $(ENVIRONMENT)
$(ENVIRONMENT): | $(PKG_BUILD_DIR)
	-rm $@
	echo "EMBOX_DERIVED_CFLAGS='$(EMBOX_DERIVED_CFLAGS)'"               >>$@
	echo "EMBOX_DERIVED_CXXFLAGS='$(EMBOX_DERIVED_CXXFLAGS)'"           >>$@
	#echo "EMBOX_DERIVED_CROSS_COMPILE='$(EMBOX_DERIVED_CROSS_COMPILE)'" >>$@
	echo "PKG_CONF_FLAGS='$(PKG_CONF_FLAGS)'"                   >>$@

# intentionally not taking into account possible changes in $(QMAKE_CONF)
configure: merge_trees $(ENVIRONMENT) | $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR) && \
		if (diff $(ENVIRONMENT) $(CONFIGURED) >/dev/null); then \
			echo Nothing to do - no reconfiguration is required.; \
		else \
			([ ! -e $(CONFIGURED) ] || (echo Removing old config ... && make confclean && rm $(CONFIGURED))) && \
			echo Configuring... && \
			$(PKG_SRC_DIR)/configure $(PKG_CONF_FLAGS) && \
			echo Persisting configuration... && \
			mv -f $(ENVIRONMENT) $(CONFIGURED); \
		fi

build_and_install: configure
	cd $(PKG_BUILD_DIR) && \
		if make -q; then \
			echo Nothing to do - no rebuild is required.; \
		else \
			echo Building... && \
			make MAKEFLAGS='$(EMBOX_DERIVED_MAKEFLAGS)';\
		fi


.NOTPARALLEL:
