#ifndef UBOOT_IPU_COMPAT_H_
#define UBOOT_IPU_COMPAT_H_
#include <hal/reg.h>
#include <kernel/irq.h>
#include <stdint.h>

typedef uint32_t resource_size_t;

#define __raw_readl(addr) REG32_LOAD(addr)
#define readl(addr) REG32_LOAD(addr)
#define __raw_writel(value, addr) REG32_STORE(addr, value)
#define writel(value, addr) REG32_STORE(addr, value)

/*  IPU Pixel format definitions */
#define fourcc(a, b, c, d)\
	(((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

/*
 * Pixel formats are defined with ASCII FOURCC code. The pixel format codes are
 * the same used by V4L2 API.
 */

#define IPU_PIX_FMT_GENERIC fourcc('I', 'P', 'U', '0')
#define IPU_PIX_FMT_GENERIC_32 fourcc('I', 'P', 'U', '1')
#define IPU_PIX_FMT_LVDS666 fourcc('L', 'V', 'D', '6')
#define IPU_PIX_FMT_LVDS888 fourcc('L', 'V', 'D', '8')

#define IPU_PIX_FMT_RGB332  fourcc('R', 'G', 'B', '1')	/*<  8  RGB-3-3-2    */
#define IPU_PIX_FMT_RGB555  fourcc('R', 'G', 'B', 'O')	/*< 16  RGB-5-5-5    */
#define IPU_PIX_FMT_RGB565  fourcc('R', 'G', 'B', 'P')	/*< 1 6  RGB-5-6-5   */
#define IPU_PIX_FMT_RGB666  fourcc('R', 'G', 'B', '6')	/*< 18  RGB-6-6-6    */
#define IPU_PIX_FMT_BGR666  fourcc('B', 'G', 'R', '6')	/*< 18  BGR-6-6-6    */
#define IPU_PIX_FMT_BGR24   fourcc('B', 'G', 'R', '3')	/*< 24  BGR-8-8-8    */
#define IPU_PIX_FMT_RGB24   fourcc('R', 'G', 'B', '3')	/*< 24  RGB-8-8-8    */
#define IPU_PIX_FMT_BGR32   fourcc('B', 'G', 'R', '4')	/*< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_BGRA32  fourcc('B', 'G', 'R', 'A')	/*< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_RGB32   fourcc('R', 'G', 'B', '4')	/*< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_RGBA32  fourcc('R', 'G', 'B', 'A')	/*< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_ABGR32  fourcc('A', 'B', 'G', 'R')	/*< 32  ABGR-8-8-8-8 */

/* YUV Interleaved Formats */
#define IPU_PIX_FMT_YUYV    fourcc('Y', 'U', 'Y', 'V')	/*< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_UYVY    fourcc('U', 'Y', 'V', 'Y')	/*< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_Y41P    fourcc('Y', '4', '1', 'P')	/*< 12 YUV 4:1:1 */
#define IPU_PIX_FMT_YUV444  fourcc('Y', '4', '4', '4')	/*< 24 YUV 4:4:4 */

/* two planes -- one Y, one Cb + Cr interleaved  */
#define IPU_PIX_FMT_NV12    fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */

#define IPU_PIX_FMT_GREY    fourcc('G', 'R', 'E', 'Y')	/*< 8  Greyscale */
#define IPU_PIX_FMT_YVU410P fourcc('Y', 'V', 'U', '9')	/*< 9  YVU 4:1:0 */
#define IPU_PIX_FMT_YUV410P fourcc('Y', 'U', 'V', '9')	/*< 9  YUV 4:1:0 */
#define IPU_PIX_FMT_YVU420P fourcc('Y', 'V', '1', '2')	/*< 12 YVU 4:2:0 */
#define IPU_PIX_FMT_YUV420P fourcc('I', '4', '2', '0')	/*< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV420P2 fourcc('Y', 'U', '1', '2')	/*< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YVU422P fourcc('Y', 'V', '1', '6')	/*< 16 YVU 4:2:2 */
#define IPU_PIX_FMT_YUV422P fourcc('4', '2', '2', 'P')	/*< 16 YUV 4:2:2 */

typedef uint32_t dma_addr_t;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

extern void ipu_dmfc_set_wait4eot(int dma_chan, int width);
extern void ipu_dmfc_init(int dmfc_type, int first);
#define debug log_debug

#define DEBUG

#define HIGH_RESOLUTION_WIDTH   1024
#define IDMA_CHAN_INVALID       0xFF

enum ipu_dmfc_type {
	DMFC_NORMAL = 0,
	DMFC_HIGH_RESOLUTION_DC,
	DMFC_HIGH_RESOLUTION_DP,
	DMFC_HIGH_RESOLUTION_ONLY_DP,
};

static __inline__ int test_bit(int nr, const volatile void *addr)
{
	return ((1UL << (nr & 31)) & (((const unsigned int *) addr)[nr >> 5])) != 0;
}

static __inline__ unsigned long ffz(unsigned long word)
{
        unsigned long result = 0;

        while (word & 1) {
                result++;
                word >>= 1;
        }
        return result;
}

#define irqreturn_t irq_return_t

extern int clk_enable(char *clk_name);
extern int clk_disable(char *clk_name);

#endif /* UBOOT_IPU_COMPAT_H_ */
