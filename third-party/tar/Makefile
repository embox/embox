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

PKG_CONF_FLAGS :=
PKG_CONF_FLAGS += --build=$(shell gcc -v 2>&1 | grep Target | awk '{print $$2}')
PKG_CONF_FLAGS += --host=$(shell ECC="$(EMBOX_CROSS_COMPILE)"; echo -n $${ECC:-i386-} | head -c -1)
PKG_CONF_FLAGS += --target=$(shell ECC="$(EMBOX_CROSS_COMPILE)"; echo -n $${ECC:-i386-} | head -c -1)

EMBOX_IMPORTED_CPPFLAGS :=
EMBOX_IMPORTED_CPPFLAGS += -I$(shell pwd)/include
EMBOX_IMPORTED_CPPFLAGS += $(filter -I%,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -nostdinc,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -D__EMBOX__,$(EMBOX_CPPFLAGS))
EMBOX_IMPORTED_CPPFLAGS += $(filter -Uarm,$(EMBOX_CPPFLAGS))

EMBOX_IMPORTED_CFLAGS :=
EMBOX_IMPORTED_CFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CFLAGS += $(filter -g,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-common,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-omit-frame-pointer,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-optimize-sibling-calls,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -fno-stack-protector,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -O%,$(EMBOX_CFLAGS))
# architecture dependent options
EMBOX_IMPORTED_CFLAGS += $(filter -m32,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mapcs,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -march%,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -marm,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mlittle-endian,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mno-thumb-interwork,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CFLAGS))
EMBOX_IMPORTED_CFLAGS += $(filter -msoft-float, $(EMBOX_CFLAGS))

EMBOX_IMPORTED_CXXFLAGS :=
EMBOX_IMPORTED_CXXFLAGS += $(EMBOX_IMPORTED_CPPFLAGS)
EMBOX_IMPORTED_CXXFLAGS += $(filter -g,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-common,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-exceptions,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-omit-frame-pointer,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-optimize-sibling-calls,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-rtti,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-stack-protector,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -fno-threadsafe-statics,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -O%,$(EMBOX_CXXFLAGS))
# architecture dependent options
EMBOX_IMPORTED_CXXFLAGS += $(filter -m32,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mapcs,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -march%,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -marm,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mlittle-endian,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-thumb-interwork,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -mno-unaligned-access,$(EMBOX_CXXFLAGS))
EMBOX_IMPORTED_CXXFLAGS += $(filter -msoft-float, $(EMBOX_CXXFLAGS))

EMBOX_IMPORTED_LDFLAGS :=
EMBOX_IMPORTED_LDFLAGS += $(filter -static,$(EMBOX_LDFLAGS))
EMBOX_IMPORTED_LDFLAGS += $(filter -nostdlib,$(EMBOX_LDFLAGS))
ifneq (,$(filter -m%,$(EMBOX_LDFLAGS)))
EMBOX_IMPORTED_LDFLAGS += -Wl,$(filter -m%,$(EMBOX_LDFLAGS))
endif
EMBOX_IMPORTED_LDFLAGS += $(shell pwd)/stubs.c
EMBOX_IMPORTED_LDFLAGS += -Wl,--relax
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=__symbol_table=0,--defsym=__symbol_table_size=0
#EMBOX_IMPORTED_LDFLAGS += -Wl,-T,$(ROOT_DIR)/build/base/obj/mk/image.lds
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_ram_base=0,--defsym=_ram_size=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_traps_text_start=0,--defsym=_traps_text_end=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_text_vma=0,--defsym=_text_lma=0,--defsym=_text_len=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_rodata_vma=0,--defsym=_rodata_lma=0,--defsym=_rodata_len=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_data_vma=0,--defsym=_data_lma=0,--defsym=_data_len=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_bss_vma=0,--defsym=_bss_lma=0,--defsym=_bss_len=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_ctors_start=0,--defsym=_ctors_end=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_dtors_start=0,--defsym=_dtors_end=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=__EH_FRAME_BEGIN__=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_bss_end=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_reserve_end=0
EMBOX_IMPORTED_LDFLAGS += -Wl,--defsym=_bss_len=0,--defsym=_bss_len_with_reserve=0

EMBOX_IMPORTED_MAKEFLAGS :=
ifneq (,$(filter -j,$(EMBOX_MAKEFLAGS)))
EMBOX_IMPORTED_MAKEFLAGS += -j $(shell nproc)
endif

.PHONY: all
all: build

.PHONY: $$(image_prerequisites)
$$(image_prerequisites):
include ../../build/base/src-gen/image.rule.mk
__define_image_rules = $(eval $(value __image_rule))
$(call __define_image_rules,all)

$(PKG_BUILD_DIR):
	mkdir -p $@

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
# but ssstypically overwriting is what we want for our part of tree contents
.PHONY: merge
merge: $(EXTRACTED) | $(PKG_SRC_DIR)
	if [ -e tree ]; then \
		cd tree && cp -l -u -f --parents -t $(PKG_SRC_DIR)/ `find . -not -path '*/.*' -and -type f`; \
	fi

.PHONY: import_ldflags
import_ldflags: _ld_objs = $(filter-out %/tar.o,$(ld_objs))
import_ldflags:
	#$(eval EMBOX_IMPORTED_LDFLAGS += $(ld_scripts:.%=-Wl,-T,$(ROOT_DIR)%))
	$(eval EMBOX_IMPORTED_LDFLAGS += $(_ld_objs:.%=-Wl,$(ROOT_DIR)%))
	$(eval EMBOX_IMPORTED_LDFLAGS += -Wl,--start-group $(ld_libs:.%=-Wl,$(ROOT_DIR)%) -Wl,--end-group)

$(ENVIRONMENT): import_ldflags | $(dir ENVIRONMENT)
	-rm $@
	echo "EMBOX_IMPORTED_CFLAGS='$(EMBOX_IMPORTED_CFLAGS)'"     >> $@
	echo "EMBOX_IMPORTED_CXXFLAGS='$(EMBOX_IMPORTED_CXXFLAGS)'" >> $@
	echo "EMBOX_IMPORTED_LDFLAGS='$(EMBOX_IMPORTED_LDFLAGS)'"   >> $@
	echo "EMBOX_CROSS_COMPILE='$(EMBOX_CROSS_COMPILE)'"         >> $@
	echo "PKG_CONF_FLAGS='$(PKG_CONF_FLAGS)'"                   >> $@

.PHONY: configure
configure: merge $(ENVIRONMENT) | $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR) && \
		if (diff $(ENVIRONMENT) $(CONFIGURED) >/dev/null); then \
			echo Nothing to do - no reconfiguration is required.; \
		else \
			([ ! -e $(CONFIGURED) ] || (echo Removing old config ... && make distclean && rm $(CONFIGURED))) && \
			echo Configuring... && \
			CFLAGS='$(EMBOX_IMPORTED_CFLAGS)' CXXFLAGS='$(EMBOX_IMPORTED_CXXFLAGS)' \
				LDFLAGS='$(EMBOX_IMPORTED_LDFLAGS)' $(PKG_SRC_DIR)/configure $(PKG_CONF_FLAGS) && \
			echo Persisting configuration... && \
			mv -f $(ENVIRONMENT) $(CONFIGURED); \
		fi

.PHONY: build
build: configure | $(PKG_BUILD_DIR)
	cd $(PKG_BUILD_DIR) && \
		if make -q; then \
			echo Nothing to do - no rebuild is required.; \
		else \
			echo Building... && \
			CFLAGS='$(EMBOX_IMPORTED_CFLAGS)' CXXFLAGS='$(EMBOX_IMPORTED_CXXFLAGS)' \
				LDFLAGS='$(EMBOX_IMPORTED_LDFLAGS)' make MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
		fi

.NOTPARALLEL:
