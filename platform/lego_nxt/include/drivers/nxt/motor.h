/**
 * @file
 * @brief NXT motor's driver interface
 *
 * @details NXT motor's interface includes functions for work  both with motor
 *          power and tachometer sensors. NXT brick has three motor's port A, B
 *          and C. First of all you should get motor's descriptor for required
 *          port (#NXT_PORT_A, #NXT_PORT_B and #NXT_PORT_C). Then you can use
 *          function #nxt_motor_set_power for start motor or functions
 *          #nxt_motor_tacho_set_counter and #nxt_motor_tacho_get_counter
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#ifndef NXT_MOTOR_H_
#define NXT_MOTOR_H_

#include <types.h>
#include <drivers/nxt/avr.h>
#include <drivers/pins.h>

/**
 * Total number of motors available in system
 */
#define NXT_N_MOTORS       NXT_AVR_N_OUTPUTS

/**
 * Gets a motor's port. You should use NXT_MOTOR_X macros to receive specific
 * port.
 * @param num - number of motor's port (0, 1, 2)
 * @return pointer to motor struct if success or NULL if error has occurred
 */
extern struct nxt_motor *nxt_get_motor(int num);

#define NXT_MOTOR_A  (nxt_get_motor(0)) /**< get structure motor on A port */
#define NXT_MOTOR_B  (nxt_get_motor(1)) /**< get structure motor on B port */
#define NXT_MOTOR_C  (nxt_get_motor(2)) /**< get structure motor on C port */

/**
 * Tacholimit handler function type. Tacholimit handler of motor
 * (motor_t.tacho_hnd) is called after
 *  motor_t.tacho_limit went to zero
 */
typedef void (*tacho_handler_t)(void);

/**
 * structure of NXT motor. it includes utility information for motors.
 * This structure is used in nxt_motor_xxx function. To use it you should get
 * a pointer to this structure with function with function #nxt_get_motor
 * or macros NXT_MOTOR_X.
 * This structure is used both in motor power function and in tachometer
 * functions.
 */
struct nxt_motor {
	uint8_t          id;

	uint32_t         tacho_limit;
	uint32_t         tacho_count;
	tacho_handler_t  limit_hnd;

	uint32_t         m_0, m_1;
	pin_handler_t    pin_handler;
};

/** typedef for #nxt_motor structure */
typedef struct nxt_motor nxt_motor_t;


/**
 * Set power motor
 * @param motor pointer to the structure @link #nxt_motor @endlink
 * @param power for motor:
 *		0    means stop
 *		-100 means full counterclockwise
 *		100	 measn full clockwise
 * */
extern void nxt_motor_set_power(nxt_motor_t *motor, int8_t power);

/**
 * Initializes all motor's preferences
 * @param motor - pointer to the structure @link #nxt_motor @endlink
 * @param limit Count of tachometer ticks before handler call
 * @param lim_handler Handler called when limit ticks passed
 */
extern void nxt_motor_tacho_set_counter(nxt_motor_t *motor, uint32_t limit,
			tacho_handler_t lim_handler);

/**
 * Gets motor tachometer counter i.e. number of motor's tachometer ticks since
 *  last tachometer limit cycle (since last tachometer handler call)
 * @param motor - pointer to the structure @link #nxt_motor @endlink
 * @return
 */
extern uint32_t nxt_motor_tacho_get_counter(nxt_motor_t *motor);

#endif /* NXT_MOTOR_H_ */
