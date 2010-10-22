/**
 * @file
 * @brief Microblaze specific bit operations
 *
 * @date 17.03.2010
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_BITOPS_H_
#define MICROBLAZE_BITOPS_H_

/* It's necessary to put 31 here
 * because microblaze has reverse bits order */
/**< macros reverts bit's number for microblaze arch*/
#define REVERSE_MASK(bit_num) (1<<(31-bit_num))

#define REVERSE_BIT(bit_num)  (31-bit_num)

#endif /* MICROBLAZE_BITOPS_H_ */
