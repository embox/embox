/**
 * @file clint.h
 *
 * @brief Function definitions for the RISC-V Core Local Interruptor (CLINT).
 *
 * @date 05.07.2024
 * @authored by Suraj Ravindra Sonawane
 */

#ifndef CLINT_H_
#define CLINT_H_

#include <stdint.h>

extern int clint_init(void);

extern void configure_soft_int(uint8_t value, int hart_id);

extern void get_timecmp(uint64_t value, int hart_id);

extern void set_timecmp(uint64_t value, int hart_id);

extern uint64_t get_current_time(void);

#endif /* CLINT_H_ */
