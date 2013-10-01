
$(OBJ_DIR)/third-party/samba/../../../../samba/src/samba/source3/bin/smbclient.embox.o: $(OBJ_DIR)/third-party/samba/../../../../samba/lib/libsmbclient.a

$(OBJ_DIR)/third-party/samba/../../../../samba/src/samba/examples/libsmbclient/testbrowse2.c: $(OBJ_DIR)/third-party/samba/../../../../samba/lib/libsmbclient.a

$(OBJ_DIR)/third-party/samba/../../../../samba/lib/libsmbclient.a: $(OBJ_DIR)/third-party/samba/../../../../samba/lib/libtevent.a

$(OBJ_DIR)/third-party/samba/../../../../samba/lib/libtevent.a: $(OBJ_DIR)/third-party/samba/../../../../samba/lib/libtalloc.a

$(OBJ_DIR)/third-party/samba/../../../../samba/lib/libtalloc.a: $(OBJ_DIR)/third-party/samba/../../../../samba/lib/libtdb.a

$(OBJ_DIR)/third-party/samba/../../../../samba/lib/libtdb.a: $(OBJ_DIR)/third-party/samba/../../../../samba/lib/libwbclient.a
