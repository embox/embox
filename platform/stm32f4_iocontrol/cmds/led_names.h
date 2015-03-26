#ifndef STM32F4_IOCONTROL_CMDS_LED_NAMES_H_
#define STM32F4_IOCONTROL_CMDS_LED_NAMES_H_

#define LEDNAME_MAX 32 /* restriction is 16 chars, which can be unicode */
#define LEDNAME_N 40
extern char led_names[LEDNAME_N][LEDNAME_MAX + 1];

#endif /* STM32F4_IOCONTROL_CMDS_LED_NAMES_H_ */
