include(ExternalProject)
ExternalProject_Add(
   samba
   PREFIX ..
   DOWNLOAD_DIR ../../../download
   URL http://www.samba.org/samba/ftp/stable/samba-4.0.8.tar.gz
   URL_MD5 03819bc1132e417feac1b39299365b73
   UPDATE_COMMAND ""
   PATCH_COMMAND
      cd $ENV{EMBOX_ROOT}/third-party/samba/tree &&
      # bash -c "cp -l -u -f --parents -t <SOURCE_DIR>/ `find . -not -path '*/.*' -and -type f`" &&
      patch -p1 -d <SOURCE_DIR> -N --input=$ENV{EMBOX_ROOT}/third-party/samba/patch.txt
   BUILD_IN_SOURCE 1
   LOG_CONFIGURE 1
   CONFIGURE_COMMAND
      <SOURCE_DIR>/configure
        ""CFLAGS=$ENV{EMBOX_DERIVED_CFLAGS}""
        ""CXXFLAGS=$ENV{EMBOX_DERIVED_CXXFLAGS}""
        ""LDFLAGS=$ENV{EMBOX_DERIVED_LDFLAGS}""
        --cross-compile
        --cross-answers=$ENV{EMBOX_ROOT}/third-party/samba/embox_ca.txt
      	--prefix=<INSTALL_DIR>
      	--bundled-libraries=tdb
      	--with-shared-modules=NONE
      	--without-ldap
      	--without-pam
      	#--with-static-modules=
      	#--targets=
   #	BUILD_COMMAND
   LOG_BUILD 1
)
