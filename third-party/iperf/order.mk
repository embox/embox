MAKE_IPERF = \
	$(MAKE) -C $(THIRDPARTY_DIR)/iperf \
	MAKEFLAGS= \
	EMBOX_ARCH='$(ARCH)' \
	EMBOX_CROSS_COMPILE='$(CROSS_COMPILE)' \
	EMBOX_MAKEFLAGS='$(MAKEFLAGS)' \
	ROOT_DIR=$(abspath $(ROOT_DIR)) \
	EMBOX_CFLAGS='$(CFLAGS)' \
	EMBOX_CXXFLAGS='$(CXXFLAGS)' \
	EMBOX_LDFLAGS='$(LDFLAGS)' \
	EMBOX_CPPFLAGS='$(EMBOX_EXPORT_CPPFLAGS)'

#MISC_DIR= $(OBJ_DIR)/third-party/iperf/../../../../iperf/src
#IPERF_DIR= $(OBJ_DIR)/third-party/iperf/../../../../iperf/src
IPERF_LIB= $(OBJ_DIR)/third-party/iperf/../../../../iperf/src/libiperf.a

$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf3-main.o: $(IPERF_LIB)
