
ifeq ($(strip $(ROOT_DIR)),)
$(error ROOT_DIR is not set)
endif

ifeq ($(strip $(MOD_BUILD_DIR)),)
$(error MOD_BUILD_DIR is not set)
endif

ifneq (,$(wildcard $(MOD_DIR)/Makefile))
extbld_makefile := $(MOD_DIR)/Makefile
else ifneq (,$(wildcard $(MOD_DIR)/makefile))
extbld_makefile := $(MOD_DIR)/makefile
else
$(error Makefile not found in directory "$(MOD_DIR)")
endif

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

PKG_SOURCE_DIR    = $(filter-out %/install,$(wildcard $(MOD_BUILD_DIR)/*))
PKG_INSTALL_DIR  := $(MOD_BUILD_DIR)/install
DOWNLOAD_BASEDIR := $(ROOT_DIR)/download

ifeq ($(value PKG_DOWNLOADS_SEPARATE),)
DOWNLOAD_DIR := $(DOWNLOAD_BASEDIR)
else
DOWNLOAD_DIR := $(DOWNLOAD_BASEDIR)/$(PKG_NAME)
endif

DOWNLOAD       := $(MOD_BUILD_DIR)/.downloaded
DOWNLOAD_CHECK := $(MOD_BUILD_DIR)/.download_checked
EXTRACT        := $(MOD_BUILD_DIR)/.extracted
PATCH          := $(MOD_BUILD_DIR)/.patched
CONFIGURE      := $(MOD_BUILD_DIR)/.configured
BUILD          := $(MOD_BUILD_DIR)/.built
INSTALL        := $(MOD_BUILD_DIR)/.installed

.PHONY : all download extract patch configure build install
all : download extract patch configure build install

$(DOWNLOAD_DIR) $(MOD_BUILD_DIR) $(PKG_INSTALL_DIR):
	$(MKDIR) $@

$(DOWNLOAD) : | $(DOWNLOAD_DIR) $(MOD_BUILD_DIR)
	$(foreach d,$(sources_archive), \
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

download : $(DOWNLOAD_CHECK)
$(DOWNLOAD_CHECK) : $(DOWNLOAD) | $(DOWNLOAD_DIR) $(MOD_BUILD_DIR)
    # check md5sum only for source archives and skip the check for git repos
	if [ "$(sources_archive)" ] ; then \
		cd $(DOWNLOAD_DIR) && ( \
				$(MD5) $(pkg_archive_name) | $(AWK) '{print $$1}' | grep $(PKG_MD5) 2>&1 >/dev/null; \
			); \
    fi;
	touch $@

extract : $(EXTRACT)
$(EXTRACT) : $(DOWNLOAD) | $(DOWNLOAD_DIR) $(MOD_BUILD_DIR)
	$(if $(first_url),$(if $(filter %zip,$(pkg_archive_ext)), \
		unzip -q $(DOWNLOAD_DIR)/$(pkg_archive_name) -d $(MOD_BUILD_DIR);, \
		$(if $(filter-out %tar.gz %tgz,$(filter %gz,$(pkg_archive_ext))), \
		gzip -dk $(DOWNLOAD_DIR)/$(pkg_archive_name); mv $(DOWNLOAD_DIR)/$(PKG_NAME) $(MOD_BUILD_DIR);, \
		tar -xf $(DOWNLOAD_DIR)/$(pkg_archive_name) -C $(MOD_BUILD_DIR);)))
	COPY_FILES="$(addprefix $(DOWNLOAD_DIR)/, \
			$(call targets_git,$(sources_git)))"; \
		if [ "$$COPY_FILES" ]; then \
			cp -R $$COPY_FILES $(MOD_BUILD_DIR); \
		fi;
	touch $@

patch : $(PATCH)
$(PATCH) : $(EXTRACT) $(PKG_PATCHES) | $(MOD_BUILD_DIR)
	if [ -d tree ]; then \
		cd tree; \
		cp -l -u -f --parents -t $(PKG_SOURCE_DIR) `find . -not -path '*/.*' -and -type f`; \
		cd ..; \
	fi
	for i in $(PKG_PATCHES); do \
		patch -d $(MOD_BUILD_DIR) -p0 < $$PWD/$$i; \
	done
	touch $@

configure : $(CONFIGURE)
$(CONFIGURE) : $(PATCH) | $(MOD_BUILD_DIR)

build : $(BUILD)
$(BUILD) : $(CONFIGURE) | $(MOD_BUILD_DIR)

install : $(INSTALL)
$(INSTALL) : $(BUILD) | $(MOD_BUILD_DIR) $(PKG_INSTALL_DIR)

include $(extbld_makefile)

PKG_NAME    ?=
PKG_VER     ?=
PKG_SOURCES ?=
PKG_EXT     ?=
PKG_MD5     ?=
PKG_PATCHES ?=

targets_git      = $(basename $(notdir $1))
sources_git     := $(filter %.git,$(PKG_SOURCES))
sources_archive := $(filter-out %.git,$(PKG_SOURCES))
first_url       := $(word 1,$(sources_archive))

ifneq ($(PKG_EXT),)
pkg_archive_ext := .$(PKG_EXT)
else ifneq ($(filter %.tar.gz %.tar.bz %.tar.bz2 %.tar.xz,$(first_url)),)
pkg_archive_ext := .tar$(suffix $(first_url))
else ifneq ($(filter %.tgz %.tbz %.zip %.gz,$(first_url)),)
pkg_archive_ext := $(suffix $(first_url))
else
pkg_archive_ext :=
endif

ifneq ($(PKG_VER),)
pkg_archive_name := $(PKG_NAME)-$(PKG_VER)$(pkg_archive_ext)
else
pkg_archive_name := $(PKG_NAME)$(pkg_archive_ext)
endif
