
ifeq ($(ROOT_DIR),)
$(error ROOT_DIR is not set.)
endif

QT_URL= http://releases.qt-project.org/qt4/source/qt-everywhere-opensource-src-4.8.4.tar.gz
QT_BUNDLE = $(notdir $(QT_URL))
QT_MD5 = 89c5ecba180cae74c66260ac732dc5cb  $(QT_BUNDLE)
# configure -embedded x86 -xplatform qws/linux-embox-g++ -no-exceptions -debug -static -little-endian -no-pch -no-largefile -no-qt3support -fast -opensource -confirm-license -no-kbd-tty -no-mouse-pc -no-mouse-linuxtp -no-gfx-linuxfb -no-gfx-multiscreen -qt-gfx-vnc -no-feature-TEMPORARYFILE -no-feature-PROCESS -no-feature-FILESYSTEMWATCHER -no-feature-SHAREDMEMORY -no-feature-NETWORKINTERFACE -no-feature-HOSTINFO -no-feature-QWS_INPUTMETHODS -no-sql-sqlite -D QT_NO_BEARERMANAGEMENT
# # these might be useful if do not want to build examples: "-nomake examples -nomake demos"

# # either this or do not build examples at all
# make sub-translations-make_default-ordered -j 5
# # examples ?and demos? are not fully compilable
# make -k
# # either this or do not build examples at all
# make sub-translations-install_subtargets-ordered install_qmake install_mkspecs

QT_DIR         := $(ROOT_DIR)/build/qt
QT_SRC_DIR     := $(QT_DIR)/source
QT_BUILD_DIR   := $(QT_DIR)/build
QT_INSTALL_DIR := $(QT_DIR)/install

PATCH_TXT = qt_patch.txt

EXTRACTED = $(QT_SRC_DIR)/.extracted

.PHONY: all install build configure

AT:=

.downloaded:
	$(AT)wget -c $(QT_URL)
	$(AT)(echo '$(QT_MD5)' | md5sum -c)
	$(AT)touch $@

$(EXTRACTED): .downloaded
	-$(AT)rm -rf $(QT_SRC_DIR)
	$(AT)mkdir -p $(QT_SRC_DIR)
	$(AT)cd $(QT_SRC_DIR) && tar -xzf $(abspath $(QT_BUNDLE))
	$(AT)cd $(QT_SRC_DIR) && mv `ls`/* ./
	$(AT)patch -p1 -d $(QT_SRC_DIR) -N --input=$(abspath $(PATCH_TXT))
	$(AT)touch $@

build: | $(EXTRACTED)
	$(AT)cp -r -s -u tree/* $(QT_SRC_DIR)/

install: build

all: install
