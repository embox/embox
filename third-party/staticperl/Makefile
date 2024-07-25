
PKG_NAME := App-Staticperl
PKG_VER  := 1.46

PKG_SOURCES := https://cpan.metacpan.org/authors/id/M/ML/MLEHMANN/$(PKG_NAME)-$(PKG_VER).tar.gz
PKG_MD5     := 127aee4c5ecb2935b3ee631c48827d69

PKG_PATCHES := staticperl.patch

include $(EXTBLD_LIB)

PERL_VERSION := https://www.cpan.org/src/5.0/perl-5.8.9.tar.bz2

PERL_CPPFLAGS := \
	-DPERL_DISABLE_PMC \
	-DPERL_ARENA_SIZE=1008 \
	-DNO_PERL_MALLOC_ENV \
	-DMSG_PROXY=MSG_PROBE \
	-DNO_LOCALE \
	-Dfork=vfork \
	-Dexecvp=execv

STATICPERL     = $(PKG_SOURCE_DIR)/.staticperl
STATICPERL_TMP = $(PKG_SOURCE_DIR)/.staticperl.tmp
PERL_SRC       = $(STATICPERL)/src/perl

$(CONFIGURE) :
	export STATICPERL=$(STATICPERL_TMP); \
	export PERL_VERSION=$(PERL_VERSION); \
	cd $(PKG_SOURCE_DIR) && ( \
		./bin/staticperl configure \
	)
	cp -fr $(STATICPERL_TMP) $(STATICPERL)
	cd $(PERL_SRC) && ( \
		curl -L -O https://github.com/arsv/perl-cross/releases/download/1.4/perl-cross-1.4.tar.gz && \
		tar --strip-components=1 -zxf perl-cross-1.4.tar.gz \
	)
	patch -d $(PERL_SRC) -p0 < perl5_8_9.patch
	patch -d $(PERL_SRC) -p0 < perl_cross.patch
	export EMBOX_GCC_LINK=full; \
	cd $(PERL_SRC) && ( \
		./cnf/configure \
			-Darchname=$(AUTOCONF_ARCH) \
			-Dosname=none \
			-Dosvers='' \
			-Ddrand01='rand()' \
			-Drandfunc=rand \
			-Dseedfunc=srand \
			-Dcc=$(EMBOX_GCC) \
			-Dar=$(EMBOX_CROSS_COMPILE)ar \
			-Dranlib=$(EMBOX_CROSS_COMPILE)ranlib \
			-Dreadelf=$(EMBOX_CROSS_COMPILE)readelf \
			-Dobjdump=$(EMBOX_CROSS_COMPILE)objdump \
			-Dccflags='$(PERL_CPPFLAGS)' \
			-Dusemymalloc=n \
			-Duseperlio \
			-Uuselargefiles \
			-Uuse64bitint \
			-Uusedl \
			-Uusethreads \
			-Uuseithreads \
			-Uusemultiplicity \
			-Uusesfio \
			-Uuseshrplib \
			-Ud_procselfexe \
			-Ui_termios \
	)
	cd $(PERL_SRC) && ( \
		sh ./Configure -S \
	)
	touch $@

$(BUILD) :
	export STATICPERL=$(STATICPERL_TMP); \
	cd $(PKG_SOURCE_DIR) && ( \
		./bin/staticperl install \
	)
	mkdir $(PKG_SOURCE_DIR)/perl_bins
	cp -f $(STATICPERL_TMP)/src/perl/perl $(PKG_SOURCE_DIR)/perl_bins
	cp -f $(STATICPERL_TMP)/src/perl/miniperl $(PKG_SOURCE_DIR)/perl_bins
	cd $(PERL_SRC) && ( \
		$(MAKE) all install \
	)
	rm -fr $(STATICPERL)/perl/bin
	cp -frnT $(STATICPERL_TMP) $(STATICPERL)
	touch $@

$(INSTALL) :
	echo '#!/bin/sh' > $(PKG_INSTALL_DIR)/staticperl.sh
	echo '' >> $(PKG_INSTALL_DIR)/staticperl.sh
	echo 'export STATICPERL=$(STATICPERL) ' >> $(PKG_INSTALL_DIR)/staticperl.sh
	echo '$(PKG_SOURCE_DIR)/bin/staticperl "$$@"' >> $(PKG_INSTALL_DIR)/staticperl.sh
	chmod +x $(PKG_INSTALL_DIR)/staticperl.sh
	touch $@
