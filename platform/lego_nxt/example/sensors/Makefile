
# Auto-generated package declaration inherited from parent directory.
# TODO Suggested renaming package to ..platform.lego_nxt.example.sensors.Makefile
$_PACKAGE := lego_nxt.example

$_MODS += touch sensor sonar light angle

$_SRCS-touch += touch.c
$_DEPS-touch += $(PLATFORM).driver.touch_sensor $(PLATFORM).driver.nxt_motor

$_SRCS-sensor += sensor.c
$_DEPS-sensor += $(PLATFORM).driver.nxt_avr_init embox.driver.pins

$_SRCS-light += light.c
$_DEPS-light += $(PLATFORM).driver.nxt_avr_init embox.driver.pins

$_SRCS-sonar += sonar.c
$_DEPS-sonar += $(PLATFORM).driver.soft_i2c $(PLATFORM).driver.sonar_sensor

$_SRCS-angle += angle.c
$_DEPS-angle += $(PLATFORM).driver.soft_i2c $(PLATFORM).driver.angle_sensor
