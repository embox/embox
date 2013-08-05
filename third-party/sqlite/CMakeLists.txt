include(ExternalProject)
ExternalProject_Add(
   sqlite
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://www.sqlite.org/2013/sqlite-autoconf-3071600.tar.gz
   URL_MD5 919aedd0c84e1d8fb07129251dbe05bf
   UPDATE_COMMAND ""
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/sqlite/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      && patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/sqlite/patch.txt
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --disable-largefile
      --disable-dynamic-extensions
      --host=i386-pc-none
   #	BUILD_COMMAND
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
