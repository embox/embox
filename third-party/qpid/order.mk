
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a: $(OBJ_DIR)/third-party/e2fsprogs/../../../../e2fsprogs/src/e2fsprogs-build/lib/uuid/libuuid.a

$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a: $(OBJ_DIR)/third-party/boost/../../../../boost/lib/libboost_system.a

# This is a workaround of weird make behaviour.
# Make finds .cpp file using VPATH search with stem.
# However dependencies searches are related to stem remainder.
# Therefore the dependency should be related to 'nothing' instead
#  of $(OBJ_DIR) or $(SRCGEN_DIR)
# Otherwise make behaves like this (this is log with $(SRCGEN_DIR) related dependency:
#       Looking for an implicit rule for `build/base/obj/third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.o'.
#       Trying pattern rule with stem `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world'.
#       Trying implicit prerequisite `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.c'.
#       Trying pattern rule with stem `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world'.
#       Trying implicit prerequisite `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.S'.
#       Trying pattern rule with stem `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world'.
#       Trying implicit prerequisite `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'.
#       Found prerequisite `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp' as VPATH `./build/base/src-gen/third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'
#       Trying rule prerequisite `build/base/obj/third-party/qpid/../../../../qpid/src/qpid/examples/messaging/.'.
#       Found an implicit rule for `build/base/obj/third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.o'.
#        Considering target file `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'.
#         Looking for an implicit rule for `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'.
#         No implicit rule found for `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'.
#         Finished prerequisites of target file `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'.
#        No need to remake target `third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'; using VPATH name `./build/base/src-gen/third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp'.
third-party/qpid/../../../../qpid/src/qpid/examples/messaging/hello_world.cpp: $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a

$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf2.a         : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf.a
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfconsole.a   : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmf2.a
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfengine.a    : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfconsole.a
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidclient.a   : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqmfengine.a
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidcommon.a   : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidclient.a
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidmessaging.a: $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidcommon.a
$(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidtypes.a    : $(OBJ_DIR)/third-party/qpid/../../../../qpid/lib/libqpidmessaging.a
