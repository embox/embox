#
# qmake configuration for building with embox-generic-g++
#

include(../../common/linux.conf)
include(../../common/gcc-base-unix.conf)
include(../../common/g++-unix.conf)
include(../../common/qws.conf)

m4_define(M4_EXTRACT_ENVVAR,`m4_patsubst(m4_esyscmd(echo $$1),`
',` ')')m4_dnl

QMAKE_CFLAGS           += M4_EXTRACT_ENVVAR(CFLAGS)
# https://bugs.launchpad.net/gcc-linaro/+bug/675347
QMAKE_CXXFLAGS         += M4_EXTRACT_ENVVAR(CXXFLAGS)

QT_CONF_FLAGS += M4_EXTRACT_ENVVAR(QT_CONF_FLAGS)
contains(QT_CONF_FLAGS, arm) {
	QMAKE_CXXFLAGS         += -fno-strict-volatile-bitfields
}
#CROSS_COMPILE          += M4_EXTRACT_ENVVAR(EMBOX_DERIVED_CROSS_COMPILE)

# Overriding
# Previous value derived from linux.conf = -lpthread
QMAKE_LIBS_THREAD  =
# Previous value derived from linux.conf = -ldl
QMAKE_LIBS_DYNLOAD =
# Just want to use these when linking executables
QMAKE_LFLAGS      += -Wl,--relocatable -nostdlib
# Not sure if it should be here but if it should
# it should be imported from Embox build settings
contains(QMAKE_CXXFLAGS, -fno-rtti) {
	QMAKE_LFLAGS      += -fno-rtti
}
contains(QMAKE_CXXFLAGS, -m32) {
	QMAKE_LFLAGS      += -m32
}

CONFIG += embox_auto_import_plugins

load(device_config)

QMAKE_CC      = M4_EXTRACT_ENVVAR(CC)
QMAKE_CXX     = M4_EXTRACT_ENVVAR(CXX)
QMAKE_LINK    = M4_EXTRACT_ENVVAR(CXX)
QMAKE_LINK_C  = M4_EXTRACT_ENVVAR(CC)
QMAKE_AR      = $${CROSS_COMPILE}ar cqs
QMAKE_LIB     = $${CROSS_COMPILE}ar -ru
QMAKE_OBJCOPY = $${CROSS_COMPILE}objcopy
QMAKE_STRIP   = $${CROSS_COMPILE}strip

load(qt_config)
