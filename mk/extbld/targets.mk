##
# This file should be included by extbld Makefiles.
#

ifeq ($(strip $(ROOT_DIR)),)
$(error ROOT_DIR is not set)
endif

ifeq ($(strip $(MOD_BUILD_DIR)),)
$(error MOD_BUILD_DIR is not set)
endif

PKG_NAME    ?=
PKG_VER     ?=
PKG_SOURCES ?=
PKG_PATCHES ?=

.PHONY : all download extract patch configure build install
all : download extract patch configure build install

PKG_SOURCE_DIR    = $(filter-out %/install,$(wildcard $(MOD_BUILD_DIR)/*))
PKG_INSTALL_DIR  := $(MOD_BUILD_DIR)/install
DOWNLOAD_BASEDIR := $(ROOT_DIR)/download

ifeq ($(value PKG_DOWNLOADS_SEPARATE),)
DOWNLOAD_DIR := $(DOWNLOAD_BASEDIR)
else
DOWNLOAD_DIR := $(DOWNLOAD_BASEDIR)/$(PKG_NAME)
endif

sources_git := $(filter %.git,$(PKG_SOURCES))
targets_git = $(basename $(notdir $1))
sources_archive_mirrors := $(filter-out %.git,$(PKG_SOURCES))

pkg_ext ?=
first_url := $(word 1,$(sources_archive_mirrors))
ifneq ($(filter %.tar.gz %.tar.bz %.tar.bz2 %.tar.xz,$(first_url)),)
	pkg_ext := .tar$(suffix $(first_url))
else ifneq ($(filter %.tgz %.tbz %.zip %.gz,$(first_url)),)
	pkg_ext := $(suffix $(first_url))
endif

ifneq ($(PKG_VER),)
	pkg_archive_name := $(PKG_NAME)-$(PKG_VER)$(pkg_ext)
else
	pkg_archive_name := $(PKG_NAME)$(pkg_ext)
endif

$(DOWNLOAD_DIR) $(MOD_BUILD_DIR) $(PKG_INSTALL_DIR):
	mkdir -p $@

DOWNLOAD := $(MOD_BUILD_DIR)/.downloaded
$(DOWNLOAD): | $(DOWNLOAD_DIR) $(MOD_BUILD_DIR)
	$(foreach d,$(sources_archive_mirrors), \
		if [ ! -f $(DOWNLOAD_DIR)/$(pkg_archive_name) ] ; then \
			cd $(DOWNLOAD_DIR); \
			curl -o $(pkg_archive_name) -f -k -L '$d' && \
			($(MD5) $(pkg_archive_name) | $(AWK) '{print $$1}' | grep $(PKG_MD5) 2>&1 >/dev/null;) || \
			$(RM) $(pkg_archive_name); \
		fi;) \
	$(foreach g,$(sources_git), \
		dir_to_clone=$(DOWNLOAD_DIR)/$(call targets_git,$g); \
		if [ ! -d $$dir_to_clone ]; then \
			if [ ! "$(PKG_VER)" ]; then \
				cd $(DOWNLOAD_DIR); \
				git clone $g; \
			else \
				mkdir $$dir_to_clone; \
				cd $$dir_to_clone; \
				git init . ; \
				git remote add origin '$g'; \
				git remote set-url origin '$g'; \
				git fetch origin ; \
				git fetch origin -t ; \
				git checkout -f -q '$(PKG_VER)'; \
				git clean -ffdx; \
			fi; \
		fi;)
	touch $@

DOWNLOAD_CHECK := $(MOD_BUILD_DIR)/.download_checked
download : $(DOWNLOAD_CHECK)
$(DOWNLOAD_CHECK) : $(DOWNLOAD) | $(DOWNLOAD_DIR) $(MOD_BUILD_DIR)
    # check md5sum only for source archives and skip the check for git repos
	if [ "$(sources_archive_mirrors)" ] ; then \
		cd $(DOWNLOAD_DIR) && ( \
				$(MD5) $(pkg_archive_name) | $(AWK) '{print $$1}' | grep $(PKG_MD5) 2>&1 >/dev/null; \
			); \
    fi;
	touch $@

EXTRACT := $(MOD_BUILD_DIR)/.extracted
extract : $(EXTRACT)
$(EXTRACT): $(DOWNLOAD) | $(DOWNLOAD_DIR) $(MOD_BUILD_DIR)
	$(if $(first_url),$(if $(filter %zip,$(pkg_ext)), \
		unzip -q $(DOWNLOAD_DIR)/$(pkg_archive_name) -d $(MOD_BUILD_DIR);, \
		$(if $(filter-out %tar.gz %tgz,$(filter %gz,$(pkg_ext))), \
		gzip -dk $(DOWNLOAD_DIR)/$(pkg_archive_name); mv $(DOWNLOAD_DIR)/$(PKG_NAME) $(MOD_BUILD_DIR);, \
		tar -xf $(DOWNLOAD_DIR)/$(pkg_archive_name) -C $(MOD_BUILD_DIR);)))
	COPY_FILES="$(addprefix $(DOWNLOAD_DIR)/, \
			$(call targets_git,$(sources_git)))"; \
		if [ "$$COPY_FILES" ]; then \
			cp -R $$COPY_FILES $(MOD_BUILD_DIR); \
		fi;
	touch $@

PATCH := $(MOD_BUILD_DIR)/.patched
patch : $(PATCH)
$(PATCH): $(EXTRACT) $(PKG_PATCHES) | $(MOD_BUILD_DIR)
	if [ -d tree ]; then \
		cd tree; \
		cp -l -u -f --parents -t $(PKG_SOURCE_DIR) `find . -not -path '*/.*' -and -type f`; \
		cd ..; \
	fi
	for i in $(PKG_PATCHES); do \
		patch -d $(MOD_BUILD_DIR) -p0 < $$PWD/$$i; \
	done
	touch $@

CONFIGURE  := $(MOD_BUILD_DIR)/.configured
configure : $(CONFIGURE)
$(CONFIGURE): $(PATCH) | $(MOD_BUILD_DIR)

BUILD := $(MOD_BUILD_DIR)/.built
build : $(BUILD)
$(BUILD): $(CONFIGURE) | $(MOD_BUILD_DIR)

INSTALL := $(MOD_BUILD_DIR)/.installed
install : $(INSTALL)
$(INSTALL): $(BUILD) | $(MOD_BUILD_DIR) $(PKG_INSTALL_DIR)
