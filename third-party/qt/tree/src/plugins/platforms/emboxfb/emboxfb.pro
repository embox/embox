TARGET = emboxfb
include(../../qpluginbase.pri)

TEMPLATE = lib
CONFIG += staticlib

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/platforms

SOURCES += main.cpp emboxfbintegration.cpp
HEADERS += emboxfbintegration.h

sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS emboxfb.pro

include(../fb_base/fb_base.pri)
include(../fontdatabases/genericunix/genericunix.pri)

target.path += $$[QT_INSTALL_PLUGINS]/platforms
INSTALLS += target
