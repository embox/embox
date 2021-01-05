
PKG_NAME := e2fsprogs
PKG_VER  := 1.42.8

PKG_SOURCES := http://downloads.sourceforge.net/project/e2fsprogs/e2fsprogs/v$(PKG_VER)/$(PKG_NAME)-$(PKG_VER).tar.gz \
	http://download.embox.rocks/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 8ef664b6eb698aa6b733df59b17b9ed4

PKG_PATCHES := patch.txt

include $(EXTBLD_LIB)

E2FSPROGS_CPPFLAGS = -include $(ROOT_DIR)/third-party/e2fsprogs/e2fsprogs_embox_compat.h \
	-I$(ROOT_DIR)/third-party/e2fsprogs/include

$(CONFIGURE) :
	cd $(PKG_SOURCE_DIR) && autoconf && ( \
		./configure --prefix=$(PKG_INSTALL_DIR) \
			--host=$(AUTOCONF_TARGET_TRIPLET) \
			--target=$(AUTOCONF_TARGET_TRIPLET) \
			CC=$(EMBOX_GCC) \
			CPPFLAGS="$(E2FSPROGS_CPPFLAGS)" \
			--disable-tls \
			--disable-backtrace \
			--disable-elf-shlibs \
			--disable-bsd-shlibs \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

PKG_INSTALL_LIBS :
	cd $(PKG_SOURCE_DIR) && \
	$(MAKE) install-libs && \
	cp $(PKG_SOURCE_DIR)/intl/libintl.a $(PKG_SOURCE_DIR)/../install/intl

PKG_INSTALL_PROGS :
	cd $(PKG_SOURCE_DIR)/../install && \
	mkdir -p misc e2fsck && \
	cp -rf $(PKG_SOURCE_DIR)/misc/* misc && \
	cp -rf $(PKG_SOURCE_DIR)/e2fsck/* e2fsck

$(INSTALL) : PKG_INSTALL_LIBS PKG_INSTALL_PROGS
	touch $@

# This is due to $(INSTALL) : PKG_INSTALL_LIBS PKG_INSTALL_PROGS
# (above) bugs with parallel build.
.NOTPARALLEL:
