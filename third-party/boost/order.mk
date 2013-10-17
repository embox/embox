
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_atomic.a         : $(OBJ_DIR)/third-party/STLport/../../../../stlport/embox-lib/libstlportg.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_chrono.a         : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_atomic.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_context.a        : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_chrono.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_date_time.a      : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_context.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_exception.a      : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_date_time.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_filesystem.a     : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_exception.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_log.a            : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_filesystem.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_log_setup.a      : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_log.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_program_options.a: $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_log_setup.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_random.a         : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_program_options.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_signals.a        : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_random.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_system.a         : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_signals.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_thread.a         : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_system.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_timer.a          : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_thread.a
$(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_wave.a           : $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_timer.a
