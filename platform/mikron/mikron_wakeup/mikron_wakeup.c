/**
 * @file
 * @brief
 *
 * @author Anton Bondarev
 * @date   09.07.2024
 */

#include <assert.h>
#include <stdint.h>

#include <drivers/clk/mikron_pm.h>
#include <drivers/clk/mikron_rcc.h>
#include <hal/platform.h>
#include <hal/system.h>

#define HSI32M_CALIBRATION_VALUE 128
#define LSI32K_CALIBRATION_VALUE 128

#define DIVIDER_AHB              0
#define DIVIDER_APB_M            0
#define DIVIDER_APB_P            0

#define BASE_ADDR                OPTION_GET(NUMBER, base_addr)

#define WU                       ((volatile struct mikron_wakeup_regs *)BASE_ADDR)

struct mikron_wakeup_regs {
	volatile uint32_t SYS_MASK;
	volatile uint32_t SYS_LEVEL;
	volatile uint32_t SYS_POWEROFF;
	volatile uint32_t POWER_SWITCH;
	volatile uint32_t CLOCKS_BU;
	volatile uint32_t CLOCKS_SYS;
	volatile uint32_t RTC_CONRTOL;
	volatile uint32_t STOP;
};

#define WU_SYS_Mask_OFFSET               0x00
#define WU_SYS_Level_OFFSET              0x04
#define WU_SYS_PowerOff_OFFSET           0x08
#define WU_PowerSwitch_On_Bat_OFFSET     0x0C
#define WU_Clocks_OFFSET                 0x10
#define WU_RTC_Control_OFFSET            0x14
#define WU_BOOT                          0x28

// SYS_MASK
#define WU_MASK_WAKEUP_RTC_S             0
#define WU_MASK_WAKEUP_RTC_M             (1 << WU_MASK_WAKEUP_RTC_S)
#define WU_MASK_WAKEUP_EXT_S             1
#define WU_MASK_WAKEUP_EXT_M             (1 << WU_MASK_WAKEUP_EXT_S)
#define WU_MASK_SYS_RESET_LDO_S          2
#define WU_MASK_SYS_RESET_LDO_M          (1 << WU_MASK_SYS_RESET_LDO_S)
#define WU_MASK_SYS_RESET_VOLTMON_S      3
#define WU_MASK_SYS_RESET_VOLTMON_M      (1 << WU_MASK_SYS_RESET_VOLTMON_S)
#define WU_MASK_SYS_RESET_BOR_S          4
#define WU_MASK_SYS_RESET_BOR_M          (1 << WU_MASK_SYS_RESET_BOR_S)
#define WU_MASK_BU_RESET_BOR_S           5
#define WU_MASK_BU_RESET_BOR_M           (1 << WU_MASK_BU_RESET_BOR_S)

// SYS_LEVEL
#define WU_SYS_LEVEL_RTC_ALARM_S         0
#define WU_SYS_LEVEL_RTC_ALARM_M         (0x1 << WU_SYS_LEVEL_RTC_ALARM_S)
#define WU_SYS_LEVEL_EXT_LINE_S          1
#define WU_SYS_LEVEL_EXT_LINE_M          (0x1 << WU_SYS_LEVEL_EXT_LINE_S)

// SYS_POWEROFF
#define WU_SYS_POWEROFF_S                0
#define WU_SYS_POWEROFF_M                (1 << WU_SYS_POWEROFF_S)

// POWER_SWITCH
#define WU_POWER_SWITCH_EN_S             0
#define WU_POWER_SWITCH_EN_M             (1 << WU_POWER_SWITCH_EN_S)
#define WU_POWER_SWITCH_CONTROL_S        1
#define WU_POWER_SWITCH_CONTROL_M        (1 << WU_POWER_SWITCH_CONTROL_S)
#define WU_POWER_SWITCH_CONTROL_VCC_M    (0 << WU_POWER_SWITCH_CONTROL_S)
#define WU_POWER_SWITCH_CONTROL_VCC_BU_M (1 << WU_POWER_SWITCH_CONTROL_S)
#define WU_POWER_SWITCH_BATT_GOOD_S      2
#define WU_POWER_SWITCH_BATT_GOOD_M      (1 << WU_POWER_SWITCH_BATT_GOOD_S)

//CLOCKS_BU
#define WU_CLOCKS_BU_OSC32K_EN_S         0
#define WU_CLOCKS_BU_OSC32K_EN_M         (1 << WU_CLOCKS_BU_OSC32K_EN_S)
#define WU_CLOCKS_BU_LSI32K_EN_S         1
#define WU_CLOCKS_BU_LSI32K_EN_M         (1 << WU_CLOCKS_BU_LSI32K_EN_S)
#define WU_CLOCKS_BU_ADJ_LSI32K_S        2
#define WU_CLOCKS_BU_ADJ_LSI32K_M        (0xFF << WU_CLOCKS_BU_ADJ_LSI32K_S)
#define WU_CLOCKS_BU_ADJ_LSI32K(v) \
	(((v) << WU_CLOCKS_BU_ADJ_LSI32K_S) & WU_CLOCKS_BU_ADJ_LSI32K_M)
#define WU_CLOCKS_BU_RTC_CLK_MUX_S        10
#define WU_CLOCKS_BU_RTC_CLK_MUX_M        (0x3 << WU_CLOCKS_BU_RTC_CLK_MUX_S)
#define WU_CLOCKS_BU_RTC_CLK_MUX_LSI32K_M (0x1 << WU_CLOCKS_BU_RTC_CLK_MUX_S)
#define WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M (0x2 << WU_CLOCKS_BU_RTC_CLK_MUX_S)
#define WU_CLOCKS_BU_OSC32K_SM_S          14
#define WU_CLOCKS_BU_OSC32K_SM_M          (1 << WU_CLOCKS_BU_OSC32K_SM_S)

// CLOCKS_SYS
#define WU_CLOCKS_SYS_OSC32M_EN_S         0
#define WU_CLOCKS_SYS_OSC32M_EN_M         (1 << WU_CLOCKS_SYS_OSC32M_EN_S)
#define WU_CLOCKS_SYS_HSI32M_EN_S         1
#define WU_CLOCKS_SYS_HSI32M_EN_M         (1 << WU_CLOCKS_SYS_HSI32M_EN_S)
#define WU_CLOCKS_SYS_ADJ_HSI32M_S        2
#define WU_CLOCKS_SYS_ADJ_HSI32M_M        (0xFF << WU_CLOCKS_SYS_ADJ_HSI32M_S)
#define WU_CLOCKS_SYS_ADJ_HSI32M(v) \
	(((v) << WU_CLOCKS_SYS_ADJ_HSI32M_S) & WU_CLOCKS_SYS_ADJ_HSI32M_M)
#define WU_CLOCKS_SYS_FORCE_32K_CLK_S 10
#define WU_CLOCKS_SYS_FORCE_32K_CLK_M (0b11 << WU_CLOCKS_SYS_FORCE_32K_CLK_S)
#define WU_CLOCKS_SYS_FORCE_32K_CLK_OSC32K_M \
	(0b01 << WU_CLOCKS_SYS_FORCE_32K_CLK_S)
#define WU_CLOCKS_SYS_FORCE_32K_CLK_LSI32K_M \
	(0b10 << WU_CLOCKS_SYS_FORCE_32K_CLK_S)

// RTC_CONRTOL
#define WU_RTC_CONTROL_RESET_S       0
#define WU_RTC_CONTROL_RESET_SET_M   (0 << WU_RTC_CONTROL_RESET_S)
#define WU_RTC_CONTROL_RESET_CLEAR_M (1 << WU_RTC_CONTROL_RESET_S)

// STOP
#define WU_STOP_S                    0
#define WU_STOP_M                    (1 << WU_STOP_S)

/* Источники тактирования RTC */
#define PCC_RTCCLKSOURCE_NO_CLK      0b00 /* Нет источника */
#define PCC_RTCCLKSOURCE_LSI32K      0b01 /* LSI32K - источник тактирования RTC */
#define PCC_RTCCLKSOURCE_OSC32K      0b10 /* OSC32K - источник тактирования RTC */

/* Источник тактирования RTC в составе ядра*/
#define PCC_RTCCLKCPUSOURCE_NO_CLK   0b00 /* Нет источника */
#define PCC_RTCCLKCPUSOURCE_LSI32K   0b01 /* LSI32K - источник тактирования RTC */
#define PCC_RTCCLKCPUSOURCE_OSC32K   0b10 /* OSC32K - источник тактирования RTC */

#define PCC_FREQ_MONITOR_SOURCE_AUTO \
	0b00 /* Опорный источник частоты монитора частоты выбирается автоматически */
#define PCC_FREQ_MONITOR_SOURCE_LSI32K \
	0b01 /* Опорный источник частоты монитора частоты принудительно выбран как LSI32K */
#define PCC_FREQ_MONITOR_SOURCE_OSC32K \
	0b10 /* Опорный источник частоты монитора частоты принудительно выбран как OSC32K */

#define PCC_FORCE_OSC_SYS_UNFIXED \
	0 /* Источник тактирования системы не выбирается принудительно */
#define PCC_FORCE_OSC_SYS_FIXED \
	1 /* Источник тактирования системы выбирается принудительно */

/* Источник тактирования RTC выбирается автоматически. Если присутствуют оба тактовых сигнала 32K, то выбирается LSI32K */
#define PCC_RTC_CLOCK_SOURCE_AUTO 0b00
#define PCC_RTC_CLOCK_SOURCE_LSI32K \
	0b01 /* Приоритетный источник тактирования RTC - LSI32K */
#define PCC_RTC_CLOCK_SOURCE_OSC32K \
	0b10 /* Приоритетный источник тактирования RTC - OSC32K */

#define PCC_CPU_RTC_CLOCK_SOURCE_OSC32K \
	0 /* Источник тактирования RTC в составе ядра - OSC32K */
#define PCC_CPU_RTC_CLOCK_SOURCE_LSI32K \
	1 /* Источник тактирования RTC в составе ядра - LSI32K */

static int HAL_PCC_FreqMonRefSet(int Force32KClk) {
	uint32_t ref_clk_m = 0;

	switch (Force32KClk) {
	case PCC_FREQ_MONITOR_SOURCE_AUTO:
		WU->CLOCKS_SYS &= ~WU_CLOCKS_SYS_FORCE_32K_CLK_M;
		return 0;
	case PCC_FREQ_MONITOR_SOURCE_OSC32K:
		ref_clk_m = WU_CLOCKS_SYS_FORCE_32K_CLK_OSC32K_M;
		mikron_pm_wait_freq_osc32k();
		break;
	case PCC_FREQ_MONITOR_SOURCE_LSI32K:
		ref_clk_m = WU_CLOCKS_SYS_FORCE_32K_CLK_LSI32K_M;
		mikron_pm_wait_freq_lsi32k();
		break;
	default:
		return -1;
	}

	WU->CLOCKS_SYS = (WU->CLOCKS_SYS & ~(WU_CLOCKS_SYS_FORCE_32K_CLK_M))
	                 | ref_clk_m;
	for (volatile int i = 0; i < 100; i++)
		;
	return 0;
}

int HAL_PCC_SetOscSystem(uint32_t OscillatorSystem, int ForceOscSys) {
	//uint32_t sys_clk_m = 0;
	//uint32_t osc_clk_flag_m = 0;

	/* Настройка источника тактирования системы */
	switch (OscillatorSystem) {
	case PCC_OSCILLATORTYPE_HSI32M:
		//sys_clk_m = PM_AHB_CLK_MUX_HSI32M_M;
		//osc_clk_flag_m = PM_FREQ_STATUS_HSI32M_M;
		break;
	case PCC_OSCILLATORTYPE_OSC32M:
		//sys_clk_m = PM_AHB_CLK_MUX_OSC32M_M;
		mikron_pm_wait_freq_osc32m();
		mikron_pm_set_ahb_clk_mux_osc32m();
		//osc_clk_flag_m = PM_FREQ_STATUS_OSC32M_M;
		break;
	case PCC_OSCILLATORTYPE_LSI32K:
		//sys_clk_m = PM_AHB_CLK_MUX_LSI32K_M;
		//osc_clk_flag_m = PM_FREQ_STATUS_LSI32K_M;
		break;
	case PCC_OSCILLATORTYPE_OSC32K:
		//sys_clk_m = PM_AHB_CLK_MUX_OSC32K_M;
		//osc_clk_flag_m = PM_FREQ_STATUS_OSC32K_M;
		break;
	default:
		return -1;
	}

	mikron_pm_set_ahb_force_mux(ForceOscSys);

	for (volatile int i = 0; i < 100; i++)
		;

	return 0;
}

int HAL_PCC_RTCClock(int Oscillator) {
	//   uint32_t clockswitch_timeout = 0;
	uint32_t rtc_clk_m = 0;
	// uint32_t osc_clk_flag_m = 0;

	switch (Oscillator) {
	case PCC_RTC_CLOCK_SOURCE_AUTO:
		WU->CLOCKS_BU &= ~WU_CLOCKS_BU_RTC_CLK_MUX_M;
		return 0;
	case PCC_RTC_CLOCK_SOURCE_LSI32K:
		rtc_clk_m = WU_CLOCKS_BU_RTC_CLK_MUX_LSI32K_M;
		//osc_clk_flag_m = PM_FREQ_STATUS_LSI32K_M;
		mikron_pm_wait_freq_lsi32k();
		break;
	case PCC_RTC_CLOCK_SOURCE_OSC32K:
		rtc_clk_m = WU_CLOCKS_BU_RTC_CLK_MUX_OSC32K_M;
		// osc_clk_flag_m = PM_FREQ_STATUS_OSC32K_M;
		mikron_pm_wait_freq_osc32k();
		break;
	default:
		return -1;
	}

	/* Выбор источника тактирования RTC */
	WU->CLOCKS_BU = (WU->CLOCKS_BU & ~WU_CLOCKS_BU_RTC_CLK_MUX_M) | rtc_clk_m;
	WU->RTC_CONRTOL = WU_RTC_CONTROL_RESET_CLEAR_M;

	for (volatile int i = 0; i < 100; i++)
		;

	return 0;
}

int HAL_PCC_CPURTCClock(int Oscillator) {
	// uint32_t clockswitch_timeout = 0;
	// uint32_t rtc_clk_m = 0;
	// uint32_t osc_clk_flag_m = 0;

	switch (Oscillator) {
	case PCC_CPU_RTC_CLOCK_SOURCE_LSI32K:
		//rtc_clk_m = PM_CPU_RTC_CLK_MUX_LSI32K_M;
		//osc_clk_flag_m = PM_FREQ_STATUS_LSI32K_M;
		mikron_pm_wait_freq_lsi32k();
		break;
	case PCC_CPU_RTC_CLOCK_SOURCE_OSC32K:
		//rtc_clk_m = PM_CPU_RTC_CLK_MUX_OSC32K_M;
		//osc_clk_flag_m = PM_FREQ_STATUS_OSC32K_M;
		mikron_pm_wait_freq_osc32k();
		break;
	default:
		return -1;
	}

	mikron_pm_set_cpu_rtc_clk_mux_osc32k();

	for (volatile int i = 0; i < 100; i++)
		;

	return 0;
}

void mikron_wakeup_init(uint32_t osc) {
	WU->CLOCKS_SYS &= ~(0b11 << WU_CLOCKS_SYS_OSC32M_EN_S); // OSC32M & HSI32M
	WU->CLOCKS_BU &= ~(0b11 << WU_CLOCKS_BU_OSC32K_EN_S);   // OSC32K & LSI32K

	WU->CLOCKS_SYS = WU_CLOCKS_SYS_ADJ_HSI32M(HSI32M_CALIBRATION_VALUE);
	WU->CLOCKS_BU = WU_CLOCKS_BU_ADJ_LSI32K(LSI32K_CALIBRATION_VALUE);

	/* Опорный источник для монитора частоты */
	HAL_PCC_FreqMonRefSet(PCC_FREQ_MONITOR_SOURCE_OSC32K);
	HAL_PCC_SetOscSystem(PCC_OSCILLATORTYPE_OSC32M, PCC_FORCE_OSC_SYS_UNFIXED);

#if 0
    //PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    //PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    //PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    
    /* Опорный источник для монитора частоты */
    errors.FreqMonRef = HAL_PCC_FreqMonRefSet(PCC_Init->FreqMon.Force32KClk);
    
    /* Настройка источника тактирования системы */
    errors.SetOscSystem = HAL_PCC_SetOscSystem(PCC_Init->FreqMon.OscillatorSystem, PCC_Init->FreqMon.ForceOscSys);
#endif

	mikron_pm_set_ahb_div(DIVIDER_AHB);
	mikron_pm_set_apbm_div(DIVIDER_APB_M);
	mikron_pm_set_apbp_div(DIVIDER_APB_P);
	// PM->DIV_AHB = DIVIDER_AHB;
	// PM->DIV_APB_M = DIVIDER_APB_M;
	// PM->DIV_APB_P = DIVIDER_APB_P;

	HAL_PCC_RTCClock(PCC_RTC_CLOCK_SOURCE_OSC32K);
	HAL_PCC_CPURTCClock(PCC_CPU_RTC_CLOCK_SOURCE_OSC32K);

#if 0
    //PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_OSC32K;
    //PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    /* Выбор источника тактирования RTC */
    errors.RTCClock = HAL_PCC_RTCClock(PCC_Init->RTCClockSelection);
    
    /* Выбор источника тактирования RTC в составе ядра*/
    errors.CPURTCClock = HAL_PCC_CPURTCClock(PCC_Init->RTCClockCPUSelection);
#endif

	if (!(osc & PCC_OSCILLATORTYPE_HSI32M)) {
		WU->CLOCKS_SYS |= (1 << WU_CLOCKS_SYS_HSI32M_EN_S);
	}

	if (!(osc & PCC_OSCILLATORTYPE_OSC32M)) {
		WU->CLOCKS_SYS |= (1 << WU_CLOCKS_SYS_OSC32M_EN_S);
	}

	if (!(osc & PCC_OSCILLATORTYPE_LSI32K)) {
		WU->CLOCKS_BU |= (1 << WU_CLOCKS_BU_LSI32K_EN_S);
	}

	if (!(osc & PCC_OSCILLATORTYPE_OSC32K)) {
		WU->CLOCKS_BU |= (1 << WU_CLOCKS_BU_OSC32K_EN_S);
	}
}
