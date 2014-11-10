
all : led-server led-client

LED_BASE = ../cmds

led-server led-client : CFLAGS += -Wall $(shell  pkg-config --libs --cflags libmodbus)

led-server : CFLAGS += -I../cmds/
led-server : $(LED_BASE)/modbus_server.c $(LED_BASE)/libleddrv.c libleddrv_ll_stub.c
	$(CC) $(CFLAGS) $^ -o $@

clean :
	-rm led-server led-client

