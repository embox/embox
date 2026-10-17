#ifndef DRIVERS_ESP32_INTMATRIX_H_
#define DRIVERS_ESP32_INTMATRIX_H_

#include <stdint.h>

/**
 * Route an ESP32-C5 HP peripheral source (0..83) to CLIC ID 16..47.
 * Call during device initialization, with the peripheral interrupt disabled.
 * The caller owns the CLIC input and must avoid conflicting routes.
 * Preserves security configuration and does not enable the interrupt.
 * Returns 0 on success, or -EINVAL for an invalid source or destination.
 */
extern int esp32_intmatrix_route(uint32_t source, uint32_t clic_int);

#endif /* DRIVERS_ESP32_INTMATRIX_H_ */
