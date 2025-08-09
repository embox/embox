PKG_NAME := kissfft
PKG_VER  := v130

PKG_SOURCES := https://github.com/mborgerding/kissfft/archive/refs/tags/$(PKG_VER).zip
PKG_MD5 := 438ba1fef5783cc5f5f201395cc477ca

$(CONFIGURE) :
	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	
	ln -sfn $(PKG_SOURCE_DIR) $(PKG_INSTALL_DIR)/include

	
	if [ -f "$(PKG_SOURCE_DIR)/_kiss_fft_guts.h" ]; then \
	    grep -q '_KISS_FFT_GUTS_H' $(PKG_SOURCE_DIR)/_kiss_fft_guts.h || { \
	        sed -i '1i#ifndef _KISS_FFT_GUTS_H\n#define _KISS_FFT_GUTS_H\n' $(PKG_SOURCE_DIR)/_kiss_fft_guts.h; \
	        echo "\n#endif /* _KISS_FFT_GUTS_H */" >> $(PKG_SOURCE_DIR)/_kiss_fft_guts.h; \
	    }; \
	fi
	touch $@
