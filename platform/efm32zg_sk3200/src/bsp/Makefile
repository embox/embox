
URL := https://googledrive.com/host/0B7g4WwjaJX23ZExQU2hHTzYyRm8/efm32bsp_libs.tar.gz
TAR_GZ := efm32bsp_libs.tar.gz

.PHONY : all
all : .extracted

.extracted : $(TAR_GZ)
	tar -zxf $<
	touch $@

$(TAR_GZ) :
	wget -c $(URL)
