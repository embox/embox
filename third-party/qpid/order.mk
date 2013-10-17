
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a: $(OBJ_DIR)/third-party/e2fsprogs/../../../../e2fsprogs/src/e2fsprogs-build/lib/uuid/libuuid.a

$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a: $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_system.a

$(OBJ_DIR)/third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp: $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a

$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf2.a"         : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfconsole.a"   : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf2.a"
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfengine.a"    : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfconsole.a"
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidclient.a"   : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfengine.a"
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidcommon.a"   : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidclient.a"
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidmessaging.a": $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidcommon.a"
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidtypes.a"    : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidmessaging.a"

