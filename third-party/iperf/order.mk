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
IPERF_LIB= $(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/compat/libcompat.a


$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/main.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Client.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Extractor.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/gnu_getopt_long.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/gnu_getopt.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Launch.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/List.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Locale.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Listener.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/PerfSocket.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/ReportCSV.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/ReportDefault.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Reporter.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Server.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/service.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/Settings.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/SocketAddr.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/sockets.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/stdio.o: $(IPERF_LIB)
$(OBJ_DIR)/third-party/iperf/../../../../iperf/src/iperf-build/src/tcp_window_size.o: $(IPERF_LIB)


