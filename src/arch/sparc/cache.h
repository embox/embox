#ifndef __CACHE_H__
#define __CACHE_H__



inline static void cache_refresh() {
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t");
}
inline static void cache_enable() {
    register unsigned int tmp;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "or %0,0xF,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );

}

inline static void cache_disable() {
    register unsigned int tmp;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "andn %0,0xF,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_instr_enable() {
    register unsigned int tmp;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "or %0,0x3,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_instr_disable() {
    register unsigned int tmp;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "andn %0,0x3,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_data_enable() {
    register unsigned int tmp;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "or %0,0xC,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_data_disable() {
    register unsigned int tmp;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "andn %0,0xC,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

#endif // ifndef __CACHE_H__
