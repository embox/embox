include(ExternalProject)
ExternalProject_Add(
   boost
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://optimate.dl.sourceforge.net/project/boost/boost/1.54.0/boost_1_54_0.tar.gz
   URL_MD5 efbfbff5a85a9330951f243d0a46e4b9
   UPDATE_COMMAND ""
   PATCH_COMMAND
      true
      #cd $ENV{EMBOX_ROOT}/third-party/samba/tree &&
      #bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`" &&
      #patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/samba/patch.txt
   BUILD_IN_SOURCE 1
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
     ./bootstrap.sh
       --prefix=<INSTALL_DIR>
   BUILD_COMMAND
     ./b2
       -q
       ""cflags=$ENV{EMBOX_DERIVED_CFLAGS}""
       ""cxxflags=$ENV{EMBOX_DERIVED_CXXFLAGS}""
       ""linkflags=$ENV{EMBOX_DERIVED_LDFLAGS}""
       variant=debug
       link=static
       threading=multi
       address-model=32
   LOG_BUILD 1
   INSTALL_COMMAND
   	false
   LOG_INSTALL 1
)
