
all : led-server led-client

led-server led-client : CFLAGS= -Wall $(shell  pkg-config --libs --cflags libmodbus)

led-server : ../cmds/modbus_server.c
	$(CC) $(CFLAGS) $< -o $@

