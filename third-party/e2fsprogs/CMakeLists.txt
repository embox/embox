include(ExternalProject)
ExternalProject_Add(
   e2fsprogs
   PREFIX ..
   URL http://downloads.sourceforge.net/project/e2fsprogs/e2fsprogs/v1.42.8/e2fsprogs-1.42.8.tar.gz

   # URL_MD5 919aedd0c84e1d8fb07129251dbe05bf

   UPDATE_COMMAND ""

   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/e2fsprogs/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      && patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/e2fsprogs/patch.txt
      && cd <SOURCE_DIR> && autoconf && cd -

   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --host=i386-pc-none
      --disable-tls
   #	BUILD_COMMAND
   LOG_BUILD 1
)
SET (CMAKE_BUILD_TYPE "Debug")
