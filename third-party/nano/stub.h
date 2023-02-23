/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.02.23
 */
#ifndef THIRD_PARTY_NANO_STUB_H_
#define THIRD_PARTY_NANO_STUB_H_

#define getc_unlocked getc

#define mkstemps(x, y) (0)

#define flockfile(x)   ((void)0)
#define funlockfile(x) ((void)0)

#endif /* THIRD_PARTY_NANO_STUB_H_ */
