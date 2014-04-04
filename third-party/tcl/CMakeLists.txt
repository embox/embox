
cmake_minimum_required(VERSION 2.8)

include(ExternalProject)
ExternalProject_Add(
   tcl
   PREFIX .
   DOWNLOAD_DIR $ENV{EMBOX_ROOT}/download
   URL http://sourceforge.net/projects/tcl/files/Tcl/8.6.1/tcl8.6.1-src.tar.gz

   URL_MD5 aae4b701ee527c6e4e1a6f9c7399882e

   UPDATE_COMMAND ""

   PATCH_COMMAND
      patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/tcl/patch.txt

   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/unix/configure --prefix=<INSTALL_DIR>
      ""CC=$ENV{EMBOX_DERIVED_CC}""
      ""CPPFLAGS=$ENV{EMBOX_DERIVED_CPPFLAGS}""
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --host=$ENV{EMBOX_DERIVED_TARGET}
      --disable-shared
      --disable-threads
      --disable-load
      --disable-dll-unloading
      --enable-symbols

   BUILD_COMMAND
      make tclsh

   INSTALL_COMMAND
      cp <INSTALL_DIR>/src/tcl-build/tclsh <INSTALL_DIR>/src/tcl-build/tclsh.o

   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
