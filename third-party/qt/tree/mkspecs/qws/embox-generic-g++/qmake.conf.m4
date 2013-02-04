#
# qmake configuration for building with embox-generic-g++
#

include(../../common/linux.conf)
include(../../common/gcc-base-unix.conf)
include(../../common/g++-unix.conf)
include(../../common/qws.conf)

m4_define(M4_EXTRACT_ENVVAR,`m4_patsubst(m4_esyscmd(echo $$1),`
',` ')')m4_dnl

QMAKE_CFLAGS           += M4_EXTRACT_ENVVAR(EMBOX_DERIVED_CFLAGS)   -include qt_embox_compat.h "-D'__impl_x(path)=<../path>'"
QMAKE_CXXFLAGS         += M4_EXTRACT_ENVVAR(EMBOX_DERIVED_CXXFLAGS) -include qt_embox_compat.h "-D'__impl_x(path)=<../path>'" -fpermissive -fno-threadsafe-statics

# Overriding
# Previous value derived from linux.conf = -lpthread
QMAKE_LIBS_THREAD  =
# Previous value derived from linux.conf = -ldl
QMAKE_LIBS_DYNLOAD =
# Just want to use these when linking executables
QMAKE_LFLAGS      += -Wl,--relocatable -nostdlib
# Not sure if it should be here but if it should
# it should be imported from Embox build settings
QMAKE_LFLAGS      += -fno-rtti

CONFIG += embox_auto_import_plugins


load(qt_config)
