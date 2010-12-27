
$_MODS += touch_test sensor_test sonar_test

$_SRCS-touch_test += touch.c
$_DEPS-touch_test += $(PLATFORM).driver.touch_sensor $(PLATFORM).driver.nxt_motor

$_SRCS-sensor_test += sensor.c
$_DEPS-sensor_test += $(PLATFORM).driver.nxt_avr_init embox.driver.pins

$_SRCS-sonar_test += sonar.c
$_DEPS-sonar_test += $(PLATFORM).driver.soft_i2c
