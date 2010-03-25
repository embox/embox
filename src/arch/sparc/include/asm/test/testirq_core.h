/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#ifndef SPARC_TESTIRQ_CORE_H_
#define SPARC_TESTIRQ_CORE_H_

extern void testirq_save_status(uint32_t *status);

extern void testirq_restore_status(uint32_t *status);

#endif /* SPARC_TESTIRQ_CORE_H_ */
