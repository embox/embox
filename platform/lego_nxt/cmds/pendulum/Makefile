$_CPPFLAGS += -I$($PLATFORM)/include
$_MODS += without_sensors pen_light pen_angle

$_SRCS-without_sensors += without_sensors.c
$_DEPS-without_sensors += $(PLATFORM).driver.nxt_motor

$_SRCS-pen_light += pen_light.c
$_DEPS-pen_light += $(PLATFORM).driver.nxt_motor $(PLATFORM).driver.nxt_avr_init embox.driver.pins

$_SRCS-pen_angle += pen_angle.c
$_DEPS-pen_angle+= $(PLATFORM).driver.soft_i2c $(PLATFORM).driver.angle_sensor $(PLATFORM).driver.nxt_motor \
		$(PLATFORM).driver.angle_sensor
