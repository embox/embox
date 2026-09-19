/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 31.08.2026
 */

#ifndef DRIVERS_DSHOT_H_
#define DRIVERS_DSHOT_H_


#define DSHOT_CMD_MOTOR_STOP                                     (0)
#define DSHOT_CMD_BEEP1                                          (1)
#define DSHOT_CMD_BEEP2                                          (2)
#define DSHOT_CMD_BEEP3                                          (3)
#define DSHOT_CMD_BEEP4                                          (4)
#define DSHOT_CMD_BEEP5                                          (5)
#define DSHOT_CMD_ESC_INFO                                       (6)
#define DSHOT_CMD_SPIN_DIRECTION_1                               (7)
#define DSHOT_CMD_SPIN_DIRECTION_2                               (8)
#define DSHOT_CMD_3D_MODE_OFF                                    (9)
#define DSHOT_CMD_3D_MODE_ON                                     (10)
#define DSHOT_CMD_SETTINGS_REQUEST                               (11)
#define DSHOT_CMD_SAVE_SETTINGS                                  (12)
#define DSHOT_EXTENDED_TELEMETRY_ENABLE                          (13)
#define DSHOT_EXTENDED_TELEMETRY_DISABLE                         (14)
#define DSHOT_CMD_SPIN_DIRECTION_NORMAL                          (20)
#define DSHOT_CMD_SPIN_DIRECTION_REVERSED                        (21)
#define DSHOT_CMD_LED0_ON                                        (22)
#define DSHOT_CMD_LED1_ON                                        (23)
#define DSHOT_CMD_LED2_ON                                        (24)
#define DSHOT_CMD_LED3_ON                                        (25)
#define DSHOT_CMD_LED0_OFF                                       (26)
#define DSHOT_CMD_LED1_OFF                                       (27)
#define DSHOT_CMD_LED2_OFF                                       (28)
#define DSHOT_CMD_LED3_OFF                                       (29)
#define DSHOT_CMD_SIGNAL_LINE_TELEMETRY_DISABLE                  (32)
#define DSHOT_CMD_SIGNAL_LINE_TELEMETRY_ENABLE                   (33)
#define DSHOT_CMD_SIGNAL_LINE_CONTINUOUS_ERPM_TELEMETRY          (34)
#define DSHOT_CMD_SIGNAL_LINE_CONTINUOUS_ERPM_PERIOD_TELEMETRY   (35)
#define DSHOT_CMD_SIGNAL_LINE_TEMPERATURE_TELEMETRY              (42)
#define DSHOT_CMD_SIGNAL_LINE_VOLTAGE_TELEMETRY                  (43)
#define DSHOT_CMD_SIGNAL_LINE_CURRENT_TELEMETRY                  (44)
#define DSHOT_CMD_SIGNAL_LINE_CONSUMPTION_TELEMETRY              (45)
#define DSHOT_CMD_SIGNAL_LINE_ERPM_TELEMETRY                     (46)
#define DSHOT_CMD_SIGNAL_LINE_ERPM_PERIOD_TELEMETRY              (47)


#define DSHOT_MIN_THROTTLE              (48)
#define DSHOT_MAX_THROTTLE              (2047)
#define DSHOT_3D_FORWARD_MIN_THROTTLE   (1048)
#define DSHOT_RANGE                     (DSHOT_MAX_THROTTLE - DSHOT_MIN_THROTTLE)

#define DSHOT_TELEMETRY_NOEDGE          (0xfffe)
#define DSHOT_TELEMETRY_INVALID         (0xffff)

#define MIN_GCR_EDGES                   (7)
#define MAX_GCR_EDGES                   (22)

#endif /* DRIVERS_DSHOT_H_ */