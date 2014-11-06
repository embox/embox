
all : led-server led-client

led-server led-client : CFLAGS += -Wall $(shell  pkg-config --libs --cflags libmodbus)
led-server : CFLAGS += -I../cmds/

led-server : ../cmds/modbus_server.c libleddrv_stub.c
	$(CC) $(CFLAGS) $^ -o $@

