#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

typedef unsigned int __ipl_t;

inline static void ipl_init(void) {

}

inline static unsigned int ipl_save(void) {
	return 0;
}

inline static void ipl_restore(unsigned int ipl) {

}
