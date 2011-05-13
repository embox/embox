
$_CPPFLAGS += -I$($PLATFORM)/include
$_MODS += without_sensors pen_ligth pen_angle

$_SRCS-without_sensors += without_sensors.c
$_DEPS-without_sensors += $(PLATFORM).driver.nxt_motor

$_SRCS-pen_ligth += pen_ligth.c
$_DEPS-pen_ligth += $(PLATFORM).driver.nxt_motor

$_SRCS-pen_angle += pen_angle.c
$_DEPS-pen_angle+= $(PLATFORM).driver.soft_i2c $(PLATFORM).driver.angle_sensor $(PLATFORM).driver.nxt_motor
