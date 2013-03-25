include(ExternalProject)
ExternalProject_Add(
   zeromq
   PREFIX ..
   URL http://download.zeromq.org/zeromq-3.2.2.tar.gz
   URL_MD5 df68431d9300da84a1a5f9a2784e33de
   UPDATE_COMMAND ""
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/zeromq/tree
      #&& bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`"
      && patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/zeromq/patch.txt
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
      ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
      ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
      ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
      --with-poller=select
      --host=i386-pc-none
   #	BUILD_COMMAND
   LOG_BUILD 1
)
