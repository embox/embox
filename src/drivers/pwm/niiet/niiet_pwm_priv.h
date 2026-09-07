/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_PWM_NIIET_NIIET_PWM_PRIV_H_
#define DRIVERS_PWM_NIIET_NIIET_PWM_PRIV_H_

#include <stdint.h>

#include <drivers/pwm.h> /* only for struct pin_description  */

#define NIIET_PWM_CHAN_MAX    (4)
#define NIIET_PWM_DMA_BUF_SIZE    (32)

#define NIIET_PWM_DMA_NUM_OFF     (0)
#define NIIET_PWM_DMA_CHAN_OFF    (16)
#define NIIET_PWM_DMA_EN_OFF      (31)

#define NIIET_PWM_DMA_EN(desc)    \
				(desc & (1 << NIIET_PWM_DMA_EN_OFF))

#define NIIET_PWM_DMA_NUM(desc)    \
				((desc >> NIIET_PWM_DMA_NUM_OFF) & 0xFF)
#define NIIET_PWM_DMA_CHAN(desc)   \
				((desc >> NIIET_PWM_DMA_CHAN_OFF) & 0xFF)

struct niiet_pwm_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	int                           clk_div;
	uint32_t                      comp_mask;
	int                           idx;

	//void *                        dma_handle[NIIET_PWM_CHAN_MAX];
	uint32_t                      dma_buffer[NIIET_PWM_CHAN_MAX][NIIET_PWM_DMA_BUF_SIZE];

};

#endif /* DRIVERS_PWM_NIIET_NIIET_PWM_PRIV_H_ */
