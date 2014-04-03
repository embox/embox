
cmake_minimum_required(VERSION 2.6)

include(ExternalProject)
ExternalProject_Add(
   iperf
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   #URL http://stats.es.net/software/iperf-3.0.tar.gz
   URL  http://citylan.dl.sourceforge.net/project/iperf/iperf-2.0.5.tar.gz

   #URL_MD5 d041282317db8e133e0de1e26c9e903c
   #URL_MD5 d041282317db8e133e0de1e26c9e903c

   UPDATE_COMMAND ""

   # sudo apt-get install libtool needed

   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/iperf/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      && patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/iperf/patch.txt
      && cd <SOURCE_DIR> && aclocal && autoconf && cd -

   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
      ""CC=$ENV{EMBOX_DERIVED_CC}""
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --host=$ENV{EMBOX_DERIVED_TARGET}

   LOG_BUILD 1
   BUILD_COMMAND
      make

   INSTALL_COMMAND
      echo 'not install'


)
SET (CMAKE_BUILD_TYPE "Debug")
