#ifndef __CACHE_H__
#define __CACHE_H__

//TODO may be put it in config file
#define CACHE_LINE_LENGTH    32

inline static void cache_set_ctrl_reg(unsigned int ctrl_reg){
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (ctrl_reg)
                          );
}

inline static void cache_refresh(void) {
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t");
}

inline static void cache_enable(void) {
    register unsigned int tmp = 0;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "or %0,0xF,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_disable(void) {
    register unsigned int tmp = 0;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "andn %0,0xF,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_instr_enable(void) {
    register unsigned int tmp = 0;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "or %0,0x3,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_instr_disable(void) {
    register unsigned int tmp = 0;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "andn %0,0x3,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_data_enable(void) {
    register unsigned int tmp = 0;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "or %0,0xC,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

inline static void cache_data_disable(void) {
    register unsigned int tmp = 0;
    __asm__ __volatile__ ("flush\n\t"
                          "nop;nop;nop\n\t"
                          "lda [%%g0]0x2,%0\n\t"
                          "andn %0,0xC,%0\n\t"
                          "sta %0,[%%g0]0x2\n\t" : : "r" (tmp)
                          );
}

#endif // ifndef __CACHE_H__
