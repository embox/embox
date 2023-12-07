
#include <stdint.h>

#include <system_k1921vg015.h>


static void SystemCoreClockUpdate(void)
{
    #if 0
    uint32_t current_sysclk;
    uint32_t pll_refclk, pll_refdiv, pll_frac, pll_fbdiv, pll_pd0a, pll_pd0b, pll_pd1a, pll_pd1b = 1;
    current_sysclk = RCU->RCU_CLKSTAT_bit.RCU_CLKSTAT_SRC;
      pll_refclk = HSECLK_VAL;
       pll_fbdiv = RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV;
       pll_refdiv = RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV;
       pll_pd0a = RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A;
       pll_pd0b = RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B;
       pll_pd1a = RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A;
       pll_pd1b = RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B;
       if (RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN) pll_frac = RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC;
       else pll_frac = 0;

       SystemPll0Clock = (pll_refclk * (pll_fbdiv+pll_frac/(1 << 24))) / (pll_refdiv * (1+pll_pd0a) * (1+pll_pd0b));
       SystemPll1Clock = (pll_refclk * (pll_fbdiv+pll_frac/(1 << 24))) / (pll_refdiv * (1+pll_pd1a) * (1+pll_pd1b));
    switch (current_sysclk) {
    case RCU_CLKSTAT_SRC_refclk:
        SystemCoreClock = HSICLK_VAL;
        break;
    case RCU_CLKSTAT_SRC_srcclk:
        SystemCoreClock = HSECLK_VAL;
        break;
    case RCU_CLKSTAT_SRC_syspll0clk:
        SystemCoreClock = SystemPll0Clock;
        break;
    case RCU_CLKSTAT_SRC_lsiclk:
        SystemCoreClock = LSICLK_VAL;
        break;
    }
#endif
}

static void clk_init(void) {
    uint32_t timeout_counter = 0;
    uint32_t sysclk_source;

    
//select system clock
#ifdef SYSCLK_PLL
//if current system clock is pll that switch to internal
if (RCU->RCU_CLKSTAT_bit.RCU_CLKSTAT_SRC == RCU_SYSCLKCFG_SRC_syspll0clk){
    RCU->RCU_SYSCLKCFG_reg = (RCU_SYSCLKCFG_SRC_refclk << RCU_SYSCLKCFG_SRC_pos);
    // Wait switching done
    while ((RCU->RCU_CLKSTAT_bit.RCU_CLKSTAT_SRC != RCU->RCU_SYSCLKCFG_bit.RCU_SYSCLKCFG_SRC ))
      ;
}
//PLLCLK = REFCLK * (FBDIV+FRAC/2^24) / (REFDIV*(1+PD0A)*(1+PD0B))
#if (HSECLK_VAL == 10000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 10 000 000 Hz
    RCU->RCU_PLLSYSCFG1_reg = 0;          //FRAC = 0					 
    RCU->RCU_PLLSYSCFG2_reg = 100;         //FBDIV

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 100;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 4;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 9;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 4;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;
#elif (HSECLK_VAL == 12000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 25 000 000 Hz
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 50;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 3;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 2;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 5;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 3;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;

#elif (HSECLK_VAL == 16000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 12 500 000 Hz
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 50;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 3;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 7;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 7;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;

#elif (HSECLK_VAL == 20000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 25 000 000 Hz
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 100;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 4;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 3;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 7;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 4;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;
#elif (HSECLK_VAL == 24000000)
// Fout0 = 50 000 000 Hz
// Fout1 = 30 000 000 Hz
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 0;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_REFDIV = 2;
    RCU->RCU_PLLSYSCFG2_bit.RCU_PLLSYSCFG2_FBDIV = 65;
    RCU->RCU_PLLSYSCFG1_bit.RCU_PLLSYSCFG1_FRAC = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0B = 2;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD0A = 3;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1B = 7;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PD1A = 4;

    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DACEN = 1;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_BYP = 0;
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_DSMEN = 0;
#else
#error "Please define HSECLK_VAL with correct values!"
#endif
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_PLLEN = 1; 	// PLL Enable
    RCU->RCU_PLLSYSCFG0_bit.RCU_PLLSYSCFG0_FOUTEN = 1; 	// Fout0 Enable
    //Waiting for PLL to stabilize
    while (!RCU->RCU_PLLSYSSTAT_bit.RCU_PLLSYSSTAT_LOCK);
      
        //select PLL as source system clock
    sysclk_source = RCU_SYSCLKCFG_SRC_syspll0clk;

#elif defined SYSCLK_HSI
    sysclk_source = RCU_SYSCLKCFG_SRC_REFCLK;
#elif defined SYSCLK_HSE
    sysclk_source = RCU_SYSCLKCFG_SRC_SRCCLK;
#elif defined SYSCLK_LSI
    sysclk_source = RCU_SYSCLKCFG_SRC_LSICLK;
#else
#error "Please define SYSCLK source (SYSCLK_PLL | SYSCLK_HSE | SYSCLK_HSI | SYSCLK_LSI)!"
#endif

    //switch sysclk
    RCU->RCU_SYSCLKCFG_reg = (sysclk_source << RCU_SYSCLKCFG_SRC_pos);
    // Wait switching done
    while ((RCU->RCU_CLKSTAT_bit.RCU_CLKSTAT_SRC != RCU->RCU_SYSCLKCFG_bit.RCU_SYSCLKCFG_SRC ))
      ;
}


void SystemInit(void) {
    clk_init();
    SystemCoreClockUpdate();
}
