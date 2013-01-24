#
# qmake configuration for building with linux-g++
#

include(../../common/linux.conf)
include(../../common/gcc-base-unix.conf)
include(../../common/g++-unix.conf)
include(../../common/qws.conf)

m4_define(M4_EXTRACT_ENVVAR,`m4_patsubst(m4_esyscmd(echo $$1),`
',` ')')m4_dnl

QMAKE_CFLAGS           += M4_EXTRACT_ENVVAR(EMBOX_DERIVED_CFLAGS)   -include qt_embox_compat.h "-D'__impl_x(path)=<../path>'"
QMAKE_CXXFLAGS         += M4_EXTRACT_ENVVAR(EMBOX_DERIVED_CXXFLAGS) -include qt_embox_compat.h "-D'__impl_x(path)=<../path>'" -fpermissive -fno-threadsafe-statics

# overriding
# previous value derived from linux.conf = -lpthread
QMAKE_LIBS_THREAD  =
# previous value derived from linux.conf = -ldl
QMAKE_LIBS_DYNLOAD =
# Just want to use these when linking executables
QMAKE_LFLAGS      += -Wl,--relocatable -nostdlib

# Don't know how to make it the right way
#app {
#contains(TEMPLATE, .*(app)) {
!isEmpty(QMAKE_APP_FLAG) {
	QMAKE_LIBS        += -L$$[QT_INSTALL_PLUGINS]/platforms
	QTPLUGIN          += qvncgraphicssystem
# QMAKE_LFLAGS_APP        +=
}


load(qt_config)
