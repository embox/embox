# fio (Flexible I/O tester) fetched from its own repository and cross
# built with the embox compiler wrappers.
#
# The port itself lives in tree/ (os/os-embox.h) and in fio-embox.patch
# (the OS selection in os/os.h and configure, and the link flags for the
# Embox target).  The configure probes run through the embox-gcc wrapper,
# so they see the kernel's headers instead of the host's and leave out
# every feature the kernel does not have.

PKG_NAME := fio
PKG_VER := e333e45bd1e7503388d71932af047ecfd22c25da
PKG_SOURCES := https://github.com/axboe/fio.git
PKG_PATCHES := fio-embox.patch

$(CONFIGURE) :
	export EMBOX_GCC_LINK=full; \
	cd $(PKG_SOURCE_DIR) && ( \
		./configure \
			--cc=$(EMBOX_CC) \
			--disable-shm \
			--disable-tcmalloc \
	)
	touch $@

$(BUILD) :
	cd $(PKG_SOURCE_DIR) && ( \
		$(MAKE) fio MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)' \
	)
	touch $@

$(INSTALL) :
	cp $(PKG_SOURCE_DIR)/fio $(PKG_INSTALL_DIR)/fio.o
	touch $@
