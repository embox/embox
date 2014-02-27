
URL:=http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/firmware/stsw-stm32078.zip
ZIP_FILE := $(ROOT_DIR)/download/stsw-stm32078.zip
LIB_AR_PATH := an3268/stm32vldiscovery_package

TMP_DIR = $@.unzip
$(BUILD_DIR)/.copied : $(ZIP_FILE)
	-mkdir -p $@ && unzip $< -d $(TMP_DIR) >/dev/null
	cp $(TMP_DIR)/$(LIB_AR_PATH)/Libraries $(dir $@) -R
	rm -R $(TMP_DIR)

$(ZIP_FILE) :
	-mkdir -p $(dir $@)
	wget -P $(dir $@) $(URL)

