/**
 * @file
 *
 * @date Sep 15, 2017
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_
#define SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t dma_addr_t;

/*!
 * Enumeration of VDI MOTION select
 */
typedef enum {
	MED_MOTION = 0,
	LOW_MOTION = 1,
	HIGH_MOTION = 2,
} ipu_motion_sel;

/*!
 * Enumeration of DI ports for ADC.
 */
typedef enum {
	DISP0,
	DISP1,
	DISP2,
	DISP3
} display_port_t;

/*  IPU Pixel format definitions */
/*  Four-character-code (FOURCC) */
#define fourcc(a, b, c, d)\
	 (((uint32_t)(a)<<0)|((uint32_t)(b)<<8)|((uint32_t)(c)<<16)|((uint32_t)(d)<<24))

/*!
 * @name IPU Pixel Formats
 *
 * Pixel formats are defined with ASCII FOURCC code. The pixel format codes are
 * the same used by V4L2 API.
 */

/*! @{ */
/*! @name Generic or Raw Data Formats */
/*! @{ */
#define IPU_PIX_FMT_GENERIC fourcc('I', 'P', 'U', '0')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_GENERIC_32 fourcc('I', 'P', 'U', '1')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_GENERIC_16 fourcc('I', 'P', 'U', '2')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_LVDS666 fourcc('L', 'V', 'D', '6')	/*!< IPU Generic Data */
#define IPU_PIX_FMT_LVDS888 fourcc('L', 'V', 'D', '8')	/*!< IPU Generic Data */
/*! @} */
/*! @name RGB Formats */
/*! @{ */
#define IPU_PIX_FMT_RGB332  fourcc('R', 'G', 'B', '1')	/*!<  8  RGB-3-3-2    */
#define IPU_PIX_FMT_RGB555  fourcc('R', 'G', 'B', 'O')	/*!< 16  RGB-5-5-5    */
#define IPU_PIX_FMT_RGB565  fourcc('R', 'G', 'B', 'P')	/*!< 1 6  RGB-5-6-5   */
#define IPU_PIX_FMT_RGB666  fourcc('R', 'G', 'B', '6')	/*!< 18  RGB-6-6-6    */
#define IPU_PIX_FMT_BGR666  fourcc('B', 'G', 'R', '6')	/*!< 18  BGR-6-6-6    */
#define IPU_PIX_FMT_BGR24   fourcc('B', 'G', 'R', '3')	/*!< 24  BGR-8-8-8    */
#define IPU_PIX_FMT_RGB24   fourcc('R', 'G', 'B', '3')	/*!< 24  RGB-8-8-8    */
#define IPU_PIX_FMT_GBR24   fourcc('G', 'B', 'R', '3')	/*!< 24  GBR-8-8-8    */
#define IPU_PIX_FMT_BGR32   fourcc('B', 'G', 'R', '4')	/*!< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_BGRA32  fourcc('B', 'G', 'R', 'A')	/*!< 32  BGR-8-8-8-8  */
#define IPU_PIX_FMT_RGB32   fourcc('R', 'G', 'B', '4')	/*!< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_RGBA32  fourcc('R', 'G', 'B', 'A')	/*!< 32  RGB-8-8-8-8  */
#define IPU_PIX_FMT_ABGR32  fourcc('A', 'B', 'G', 'R')	/*!< 32  ABGR-8-8-8-8 */
/*! @} */
/*! @name YUV Interleaved Formats */
/*! @{ */
#define IPU_PIX_FMT_YUYV    fourcc('Y', 'U', 'Y', 'V')	/*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_UYVY    fourcc('U', 'Y', 'V', 'Y')	/*!< 16 YUV 4:2:2 */
#define IPU_PIX_FMT_YVYU    fourcc('Y', 'V', 'Y', 'U')  /*!< 16 YVYU 4:2:2 */
#define IPU_PIX_FMT_VYUY    fourcc('V', 'Y', 'U', 'Y')  /*!< 16 VYYU 4:2:2 */
#define IPU_PIX_FMT_Y41P    fourcc('Y', '4', '1', 'P')	/*!< 12 YUV 4:1:1 */
#define IPU_PIX_FMT_YUV444  fourcc('Y', '4', '4', '4')	/*!< 24 YUV 4:4:4 */
#define IPU_PIX_FMT_VYU444  fourcc('V', '4', '4', '4')	/*!< 24 VYU 4:4:4 */
/* two planes -- one Y, one Cb + Cr interleaved  */
#define IPU_PIX_FMT_NV12    fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
/* two planes -- 12  tiled Y/CbCr 4:2:0  */
#define IPU_PIX_FMT_TILED_NV12    fourcc('T', 'N', 'V', 'P')
#define IPU_PIX_FMT_TILED_NV12F   fourcc('T', 'N', 'V', 'F')

/*! @} */
/*! @name YUV Planar Formats */
/*! @{ */
#define IPU_PIX_FMT_GREY    fourcc('G', 'R', 'E', 'Y')	/*!< 8  Greyscale */
#define IPU_PIX_FMT_YVU410P fourcc('Y', 'V', 'U', '9')	/*!< 9  YVU 4:1:0 */
#define IPU_PIX_FMT_YUV410P fourcc('Y', 'U', 'V', '9')	/*!< 9  YUV 4:1:0 */
#define IPU_PIX_FMT_YVU420P fourcc('Y', 'V', '1', '2')	/*!< 12 YVU 4:2:0 */
#define IPU_PIX_FMT_YUV420P fourcc('I', '4', '2', '0')	/*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YUV420P2 fourcc('Y', 'U', '1', '2')	/*!< 12 YUV 4:2:0 */
#define IPU_PIX_FMT_YVU422P fourcc('Y', 'V', '1', '6')	/*!< 16 YVU 4:2:2 */
#define IPU_PIX_FMT_YUV422P fourcc('4', '2', '2', 'P')	/*!< 16 YUV 4:2:2 */
/* non-interleaved 4:4:4 */
#define IPU_PIX_FMT_YUV444P fourcc('4', '4', '4', 'P')	/*!< 24 YUV 4:4:4 */
/*! @} */
#define IPU_PIX_FMT_TILED_NV12_MBALIGN	(16)
#define TILED_NV12_FRAME_SIZE(w, h)	\
		(ALIGN((w) * (h), SZ_4K) + ALIGN((w) * (h) / 2, SZ_4K))
/* IPU device */
typedef enum {
	RGB_CS,
	YUV_CS,
	NULL_CS
} cs_t;

struct ipu_pos {
	uint32_t x;
	uint32_t y;
};

struct ipu_crop {
	struct ipu_pos pos;
	uint32_t w;
	uint32_t h;
};

struct ipu_deinterlace {
	bool	enable;
	uint8_t	motion; /*see ipu_motion_sel*/
#define IPU_DEINTERLACE_FIELD_TOP	0
#define IPU_DEINTERLACE_FIELD_BOTTOM	1
#define IPU_DEINTERLACE_FIELD_MASK	\
		(IPU_DEINTERLACE_FIELD_TOP | IPU_DEINTERLACE_FIELD_BOTTOM)
	/* deinterlace frame rate double flags */
#define IPU_DEINTERLACE_RATE_EN		0x80
#define IPU_DEINTERLACE_RATE_FRAME1	0x40
#define IPU_DEINTERLACE_RATE_MASK	\
		(IPU_DEINTERLACE_RATE_EN | IPU_DEINTERLACE_RATE_FRAME1)
#define IPU_DEINTERLACE_MAX_FRAME	2
	uint8_t	field_fmt;
};

struct ipu_input {
	uint32_t width;
	uint32_t height;
	uint32_t format;
	struct ipu_crop crop;
	dma_addr_t paddr;

	struct ipu_deinterlace deinterlace;
	dma_addr_t paddr_n; /*valid when deinterlace enable*/
};

struct ipu_alpha {
#define IPU_ALPHA_MODE_GLOBAL	0
#define IPU_ALPHA_MODE_LOCAL	1
	uint8_t mode;
	uint8_t gvalue; /* 0~255 */
	dma_addr_t loc_alp_paddr;
};

struct ipu_colorkey {
	bool enable;
	uint32_t value; /* RGB 24bit */
};

struct ipu_overlay {
	uint32_t	width;
	uint32_t	height;
	uint32_t	format;
	struct ipu_crop crop;
	struct ipu_alpha alpha;
	struct ipu_colorkey colorkey;
	dma_addr_t paddr;
};

struct ipu_output {
	uint32_t	width;
	uint32_t	height;
	uint32_t	format;
	uint8_t	rotate;
	struct ipu_crop crop;
	dma_addr_t paddr;
};

struct ipu_task {
	struct ipu_input input;
	struct ipu_output output;

	bool overlay_en;
	struct ipu_overlay overlay;

#define IPU_TASK_PRIORITY_NORMAL 0
#define IPU_TASK_PRIORITY_HIGH	1
	uint8_t	priority;

#define	IPU_TASK_ID_ANY	0
#define	IPU_TASK_ID_VF	1
#define	IPU_TASK_ID_PP	2
#define	IPU_TASK_ID_MAX 3
	uint8_t	task_id;

	int	timeout;
};

enum {
	IPU_CHECK_OK = 0,
	IPU_CHECK_WARN_INPUT_OFFS_NOT8ALIGN = 0x1,
	IPU_CHECK_WARN_OUTPUT_OFFS_NOT8ALIGN = 0x2,
	IPU_CHECK_WARN_OVERLAY_OFFS_NOT8ALIGN = 0x4,
	IPU_CHECK_ERR_MIN,
	IPU_CHECK_ERR_INPUT_CROP,
	IPU_CHECK_ERR_OUTPUT_CROP,
	IPU_CHECK_ERR_OVERLAY_CROP,
	IPU_CHECK_ERR_INPUT_OVER_LIMIT,
	IPU_CHECK_ERR_OV_OUT_NO_FIT,
	IPU_CHECK_ERR_OVERLAY_WITH_VDI,
	IPU_CHECK_ERR_PROC_NO_NEED,
	IPU_CHECK_ERR_SPLIT_INPUTW_OVER,
	IPU_CHECK_ERR_SPLIT_INPUTH_OVER,
	IPU_CHECK_ERR_SPLIT_OUTPUTW_OVER,
	IPU_CHECK_ERR_SPLIT_OUTPUTH_OVER,
	IPU_CHECK_ERR_SPLIT_WITH_ROT,
	IPU_CHECK_ERR_NOT_SUPPORT,
	IPU_CHECK_ERR_NOT16ALIGN,
	IPU_CHECK_ERR_W_DOWNSIZE_OVER,
	IPU_CHECK_ERR_H_DOWNSIZE_OVER,
};

#define IPU1_ERROR_IRQ  37
#define IPU1_SYNC_IRQ   38
#define IPU2_ERROR_IRQ  39
#define IPU2_SYNC_IRQ   40

#define IPU_BASE OPTION_MODULE_GET( \
		embox__driver__gpu__ipu_v3, \
		NUMBER,   \
		base_addr)

enum imx_ipu_rev {
	IPU_V3DEX = 2,
	IPU_V3M,
	IPU_V3H,
};

/*
 * hw_rev 2: IPUV3DEX
 * hw_rev 3: IPUV3M
 * hw_rev 4: IPUV3H
 */
static const int g_ipu_hw_rev = 4;

#define IPU_MAX_VDI_IN_WIDTH	({g_ipu_hw_rev >= 3 ? \
				   (968) : \
				   (720); })
#define IPU_DISP0_BASE		0x00000000
#define IPU_MCU_T_DEFAULT	8
#define IPU_DISP1_BASE		({g_ipu_hw_rev < 4 ? \
				(IPU_MCU_T_DEFAULT << 25) : \
				(0x00000000); })
#define IPUV3DEX_REG_BASE	0x1E000000
#define IPUV3M_REG_BASE		0x06000000
#define IPUV3H_REG_BASE		0x00200000

#define IPU_CM_REG_BASE		0x00000000
#define IPU_IDMAC_REG_BASE	0x00008000
#define IPU_ISP_REG_BASE	0x00010000
#define IPU_DP_REG_BASE		0x00018000
#define IPU_IC_REG_BASE		0x00020000
#define IPU_IRT_REG_BASE	0x00028000
#define IPU_CSI0_REG_BASE	0x00030000
#define IPU_CSI1_REG_BASE	0x00038000
#define IPU_DI0_REG_BASE	0x00040000
#define IPU_DI1_REG_BASE	0x00048000
#define IPU_SMFC_REG_BASE	0x00050000
#define IPU_DC_REG_BASE		0x00058000
#define IPU_DMFC_REG_BASE	0x00060000
#define IPU_VDI_REG_BASE	0x00068000
#define IPU_CPMEM_REG_BASE 	({g_ipu_hw_rev >= 4 ? \
				   (0x00100000) : \
				   (0x01000000); })
#define IPU_CPMEM_REG_LEN	0x20000
#define IPU_LUT_REG_BASE	0x01020000
#define IPU_SRM_REG_BASE 	({g_ipu_hw_rev >= 4 ? \
				   (0x00140000) : \
				   (0x01040000); })
#define IPU_TPM_REG_BASE 	({g_ipu_hw_rev >= 4 ? \
				   (0x00160000) : \
				   (0x01060000); })
#define IPU_DC_TMPL_REG_BASE	0x00180000/* ({g_ipu_hw_rev >= 4 ? \
				   (0x00180000) : \
				   (0x01080000); }) */
#define IPU_ISP_TBPR_REG_BASE	0x010C0000

/* Register addresses */
/* IPU Common registers */
#define IPU_CM_REG(offset)		(offset)

#define IPU_CONF			IPU_CM_REG(0)
#define IPU_SRM_PRI1			IPU_CM_REG(0x00A0)
#define IPU_SRM_PRI2			IPU_CM_REG(0x00A4)
#define IPU_FS_PROC_FLOW1		IPU_CM_REG(0x00A8)
#define IPU_FS_PROC_FLOW2		IPU_CM_REG(0x00AC)
#define IPU_FS_PROC_FLOW3		IPU_CM_REG(0x00B0)
#define IPU_FS_DISP_FLOW1		IPU_CM_REG(0x00B4)
#define IPU_FS_DISP_FLOW2		IPU_CM_REG(0x00B8)
#define IPU_SKIP			IPU_CM_REG(0x00BC)
#define IPU_DISP_ALT_CONF		IPU_CM_REG(0x00C0)
#define IPU_DISP_GEN			IPU_CM_REG(0x00C4)
#define IPU_DISP_ALT1			IPU_CM_REG(0x00C8)
#define IPU_DISP_ALT2			IPU_CM_REG(0x00CC)
#define IPU_DISP_ALT3			IPU_CM_REG(0x00D0)
#define IPU_DISP_ALT4			IPU_CM_REG(0x00D4)
#define IPU_SNOOP			IPU_CM_REG(0x00D8)
#define IPU_MEM_RST			IPU_CM_REG(0x00DC)
#define IPU_PM				IPU_CM_REG(0x00E0)
#define IPU_GPR				IPU_CM_REG(0x00E4)
#define IPU_CHA_DB_MODE_SEL(ch)		IPU_CM_REG(0x0150 + 4 * ((ch) / 32))
#define IPU_ALT_CHA_DB_MODE_SEL(ch)	IPU_CM_REG(0x0168 + 4 * ((ch) / 32))
/*
 * IPUv3D doesn't support triple buffer, so point
 * IPU_CHA_TRB_MODE_SEL, IPU_CHA_TRIPLE_CUR_BUF and
 * IPU_CHA_BUF2_RDY to readonly
 * IPU_ALT_CUR_BUF0 for IPUv3D.
 */
#define IPU_CHA_TRB_MODE_SEL(ch) IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0178 + 4 * ((ch) / 32)) : \
					    (0x012C); })
#define IPU_CHA_TRIPLE_CUR_BUF(ch) IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					      (0x0258 + \
					       4 * (((ch) * 2) / 32)) : \
					      (0x012C); })
#define IPU_CHA_BUF2_RDY(ch)	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0288 + 4 * ((ch) / 32)) : \
					    (0x012C); })
#define IPU_CHA_CUR_BUF(ch)	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x023C + 4 * ((ch) / 32)) : \
					    (0x0124 + 4 * ((ch) / 32)); })
#define IPU_ALT_CUR_BUF0	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0244) : \
					    (0x012C); })
#define IPU_ALT_CUR_BUF1	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0248) : \
					    (0x0130); })
#define IPU_SRM_STAT		IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x024C) : \
					    (0x0134); })
#define IPU_PROC_TASK_STAT	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0250) : \
					    (0x0138); })
#define IPU_DISP_TASK_STAT	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0254) : \
					    (0x013C); })
#define IPU_CHA_BUF0_RDY(ch)	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0268 + 4 * ((ch) / 32)) : \
					    (0x0140 + 4 * ((ch) / 32)); })
#define IPU_CHA_BUF1_RDY(ch)	IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0270 + 4 * ((ch) / 32)) : \
					    (0x0148 + 4 * ((ch) / 32)); })
#define IPU_ALT_CHA_BUF0_RDY(ch) IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					     (0x0278 + 4 * ((ch) / 32)) : \
					     (0x0158 + 4 * ((ch) / 32)); })
#define IPU_ALT_CHA_BUF1_RDY(ch) IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					     (0x0280 + 4 * ((ch) / 32)) : \
					     (0x0160 + 4 * ((ch) / 32)); })

#define IPU_INT_CTRL(n)		IPU_CM_REG(0x003C + 4 * ((n) - 1))
#define IPU_INT_STAT(n)		IPU_CM_REG({g_ipu_hw_rev >= 2 ? \
					    (0x0200 + 4 * ((n) - 1)) : \
					    (0x00E8 + 4 * ((n) - 1)); })

#define IPUIRQ_2_STATREG(irq)	IPU_CM_REG(IPU_INT_STAT(1) + 4 * ((irq) / 32))
#define IPUIRQ_2_CTRLREG(irq)	IPU_CM_REG(IPU_INT_CTRL(1) + 4 * ((irq) / 32))
#define IPUIRQ_2_MASK(irq)	(1UL << ((irq) & 0x1F))

/* IPU VDI registers */
#define IPU_VDI_REG(offset)	(offset)

#define VDI_FSIZE		IPU_VDI_REG(0)
#define VDI_C			IPU_VDI_REG(0x0004)

/* IPU CSI Registers */
#define IPU_CSI_REG(offset)	(offset)

#define CSI_SENS_CONF		IPU_CSI_REG(0)
#define CSI_SENS_FRM_SIZE	IPU_CSI_REG(0x0004)
#define CSI_ACT_FRM_SIZE	IPU_CSI_REG(0x0008)
#define CSI_OUT_FRM_CTRL	IPU_CSI_REG(0x000C)
#define CSI_TST_CTRL		IPU_CSI_REG(0x0010)
#define CSI_CCIR_CODE_1		IPU_CSI_REG(0x0014)
#define CSI_CCIR_CODE_2		IPU_CSI_REG(0x0018)
#define CSI_CCIR_CODE_3		IPU_CSI_REG(0x001C)
#define CSI_MIPI_DI		IPU_CSI_REG(0x0020)
#define CSI_SKIP		IPU_CSI_REG(0x0024)
#define CSI_CPD_CTRL		IPU_CSI_REG(0x0028)
#define CSI_CPD_RC(n)		IPU_CSI_REG(0x002C + 4 * (n))
#define CSI_CPD_RS(n)		IPU_CSI_REG(0x004C + 4 * (n))
#define CSI_CPD_GRC(n)		IPU_CSI_REG(0x005C + 4 * (n))
#define CSI_CPD_GRS(n)		IPU_CSI_REG(0x007C + 4 * (n))
#define CSI_CPD_GBC(n)		IPU_CSI_REG(0x008C + 4 * (n))
#define CSI_CPD_GBS(n)		IPU_CSI_REG(0x00AC + 4 * (n))
#define CSI_CPD_BC(n)		IPU_CSI_REG(0x00BC + 4 * (n))
#define CSI_CPD_BS(n)		IPU_CSI_REG(0x00DC + 4 * (n))
#define CSI_CPD_OFFSET1		IPU_CSI_REG(0x00EC)
#define CSI_CPD_OFFSET2		IPU_CSI_REG(0x00F0)

/* IPU SMFC Registers */
#define IPU_SMFC_REG(offset)	(offset)

#define SMFC_MAP		IPU_SMFC_REG(0)
#define SMFC_WMC		IPU_SMFC_REG(0x0004)
#define SMFC_BS			IPU_SMFC_REG(0x0008)

/* IPU IC Registers */
#define IPU_IC_REG(offset)	(offset)

#define IC_CONF			IPU_IC_REG(0)
#define IC_PRP_ENC_RSC		IPU_IC_REG(0x0004)
#define IC_PRP_VF_RSC		IPU_IC_REG(0x0008)
#define IC_PP_RSC		IPU_IC_REG(0x000C)
#define IC_CMBP_1		IPU_IC_REG(0x0010)
#define IC_CMBP_2		IPU_IC_REG(0x0014)
#define IC_IDMAC_1		IPU_IC_REG(0x0018)
#define IC_IDMAC_2		IPU_IC_REG(0x001C)
#define IC_IDMAC_3		IPU_IC_REG(0x0020)
#define IC_IDMAC_4		IPU_IC_REG(0x0024)

/* IPU IDMAC Registers */
#define IPU_IDMAC_REG(offset)	(offset)

#define IDMAC_CONF		IPU_IDMAC_REG(0x0000)
#define IDMAC_CHA_EN(ch)	IPU_IDMAC_REG(0x0004 + 4 * ((ch) / 32))
#define IDMAC_SEP_ALPHA		IPU_IDMAC_REG(0x000C)
#define IDMAC_ALT_SEP_ALPHA	IPU_IDMAC_REG(0x0010)
#define IDMAC_CHA_PRI(ch)	IPU_IDMAC_REG(0x0014 + 4 * ((ch) / 32))
#define IDMAC_WM_EN(ch)		IPU_IDMAC_REG(0x001C + 4 * ((ch) / 32))
#define IDMAC_CH_LOCK_EN_1	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x0024) : 0; })
#define IDMAC_CH_LOCK_EN_2	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x0028) : \
					       (0x0024); })
#define IDMAC_SUB_ADDR_0	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x002C) : \
					       (0x0028); })
#define IDMAC_SUB_ADDR_1	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x0030) : \
					       (0x002C); })
#define IDMAC_SUB_ADDR_2	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x0034) : \
					       (0x0030); })
/*
 * IPUv3D doesn't support IDMAC_SUB_ADDR_3 and IDMAC_SUB_ADDR_4,
 * so point them to readonly IDMAC_CHA_BUSY1 for IPUv3D.
 */
#define IDMAC_SUB_ADDR_3	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x0038) : \
					       (0x0040); })
#define IDMAC_SUB_ADDR_4	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x003C) : \
					       (0x0040); })
#define IDMAC_BAND_EN(ch)	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x0040 + 4 * ((ch) / 32)) : \
					       (0x0034 + 4 * ((ch) / 32)); })
#define IDMAC_CHA_BUSY(ch)	IPU_IDMAC_REG({g_ipu_hw_rev >= 2 ? \
					       (0x0100 + 4 * ((ch) / 32)) : \
					       (0x0040 + 4 * ((ch) / 32)); })

/* IPU DI Registers */
#define IPU_DI_REG(offset)	(offset)

#define DI_GENERAL		IPU_DI_REG(0)
#define DI_BS_CLKGEN0		IPU_DI_REG(0x0004)
#define DI_BS_CLKGEN1		IPU_DI_REG(0x0008)
#define DI_SW_GEN0(gen)		IPU_DI_REG(0x000C + 4 * ((gen) - 1))
#define DI_SW_GEN1(gen)		IPU_DI_REG(0x0030 + 4 * ((gen) - 1))
#define DI_STP_REP(gen)		IPU_DI_REG(0x0148 + 4 * (((gen) - 1) / 2))
#define DI_SYNC_AS_GEN		IPU_DI_REG(0x0054)
#define DI_DW_GEN(gen)		IPU_DI_REG(0x0058 + 4 * (gen))
#define DI_DW_SET(gen, set)	IPU_DI_REG(0x0088 + 4 * ((gen) + 0xC * (set)))
#define DI_SER_CONF		IPU_DI_REG(0x015C)
#define DI_SSC			IPU_DI_REG(0x0160)
#define DI_POL			IPU_DI_REG(0x0164)
#define DI_AW0			IPU_DI_REG(0x0168)
#define DI_AW1			IPU_DI_REG(0x016C)
#define DI_SCR_CONF		IPU_DI_REG(0x0170)
#define DI_STAT			IPU_DI_REG(0x0174)

/* IPU DMFC Registers */
#define IPU_DMFC_REG(offset)	(offset)

#define DMFC_RD_CHAN		IPU_DMFC_REG(0)
#define DMFC_WR_CHAN		IPU_DMFC_REG(0x0004)
#define DMFC_WR_CHAN_DEF	IPU_DMFC_REG(0x0008)
#define DMFC_DP_CHAN		IPU_DMFC_REG(0x000C)
#define DMFC_DP_CHAN_DEF	IPU_DMFC_REG(0x0010)
#define DMFC_GENERAL1		IPU_DMFC_REG(0x0014)
#define DMFC_GENERAL2		IPU_DMFC_REG(0x0018)
#define DMFC_IC_CTRL		IPU_DMFC_REG(0x001C)
#define DMFC_STAT		IPU_DMFC_REG(0x0020)

/* IPU DC Registers */
#define IPU_DC_REG(offset)	(offset)

#define DC_MAP_CONF_PTR(n)	IPU_DC_REG(0x0108 + ((n) & ~0x1) * 2)
#define DC_MAP_CONF_VAL(n)	IPU_DC_REG(0x0144 + ((n) & ~0x1) * 2)

#define _RL_CH_2_OFFSET(ch)	(((ch) == 0) ? 8 : ( \
				 ((ch) == 1) ? 0x24 : ( \
				 ((ch) == 2) ? 0x40 : ( \
				 ((ch) == 5) ? 0x64 : ( \
				 ((ch) == 6) ? 0x80 : ( \
				 ((ch) == 8) ? 0x9C : ( \
				 ((ch) == 9) ? 0xBC : (-1))))))))
#define DC_RL_CH(ch, evt)	IPU_DC_REG(_RL_CH_2_OFFSET(ch) + \
					   ((evt) & ~0x1) * 2)

#define DC_EVT_NF		0
#define DC_EVT_NL		1
#define DC_EVT_EOF		2
#define DC_EVT_NFIELD		3
#define DC_EVT_EOL		4
#define DC_EVT_EOFIELD		5
#define DC_EVT_NEW_ADDR		6
#define DC_EVT_NEW_CHAN		7
#define DC_EVT_NEW_DATA		8

#define DC_EVT_NEW_ADDR_W_0	0
#define DC_EVT_NEW_ADDR_W_1	1
#define DC_EVT_NEW_CHAN_W_0	2
#define DC_EVT_NEW_CHAN_W_1	3
#define DC_EVT_NEW_DATA_W_0	4
#define DC_EVT_NEW_DATA_W_1	5
#define DC_EVT_NEW_ADDR_R_0	6
#define DC_EVT_NEW_ADDR_R_1	7
#define DC_EVT_NEW_CHAN_R_0	8
#define DC_EVT_NEW_CHAN_R_1	9
#define DC_EVT_NEW_DATA_R_0	10
#define DC_EVT_NEW_DATA_R_1	11
#define DC_EVEN_UGDE0 		12
#define DC_ODD_UGDE0 		13
#define DC_EVEN_UGDE1 		14
#define DC_ODD_UGDE1 		15
#define DC_EVEN_UGDE2 		16
#define DC_ODD_UGDE2 		17
#define DC_EVEN_UGDE3 		18
#define DC_ODD_UGDE3 		19

#define dc_ch_offset(ch) \
({ \
	const uint8_t _offset[] = { \
		0, 0x1C, 0x38, 0x54, 0x58, 0x5C, 0x78, 0, 0x94, 0xB4}; \
	_offset[ch]; \
})
#define DC_WR_CH_CONF(ch)	IPU_DC_REG(dc_ch_offset(ch))
#define DC_WR_CH_ADDR(ch)	IPU_DC_REG(dc_ch_offset(ch) + 4)

#define DC_WR_CH_CONF_1		IPU_DC_REG(0x001C)
#define DC_WR_CH_ADDR_1		IPU_DC_REG(0x0020)
#define DC_WR_CH_CONF_5		IPU_DC_REG(0x005C)
#define DC_WR_CH_ADDR_5		IPU_DC_REG(0x0060)
#define DC_GEN			IPU_DC_REG(0x00D4)
#define DC_DISP_CONF1(disp)	IPU_DC_REG(0x00D8 + 4 * (disp))
#define DC_DISP_CONF2(disp)	IPU_DC_REG(0x00E8 + 4 * (disp))
#define DC_STAT			IPU_DC_REG(0x01C8)
#define DC_UGDE_0(evt)		IPU_DC_REG(0x0174 + 16 * (evt))
#define DC_UGDE_1(evt)		IPU_DC_REG(0x0178 + 16 * (evt))
#define DC_UGDE_2(evt)		IPU_DC_REG(0x017C + 16 * (evt))
#define DC_UGDE_3(evt)		IPU_DC_REG(0x0180 + 16 * (evt))

/* IPU DP Registers */
#define IPU_DP_REG(offset)		(offset)

#define DP_SYNC				0
#define DP_ASYNC0			0x60
#define DP_ASYNC1			0xBC
#define DP_COM_CONF(flow)		IPU_DP_REG(flow)
#define DP_GRAPH_WIND_CTRL(flow)	IPU_DP_REG(0x0004 + (flow))
#define DP_FG_POS(flow)			IPU_DP_REG(0x0008 + (flow))
#define DP_GAMMA_C(flow, i)		IPU_DP_REG(0x0014 + (flow) + 4 * (i))
#define DP_GAMMA_S(flow, i)		IPU_DP_REG(0x0034 + (flow) + 4 * (i))
#define DP_CSC_A_0(flow)		IPU_DP_REG(0x0044 + (flow))
#define DP_CSC_A_1(flow)		IPU_DP_REG(0x0048 + (flow))
#define DP_CSC_A_2(flow)		IPU_DP_REG(0x004C + (flow))
#define DP_CSC_A_3(flow)		IPU_DP_REG(0x0050 + (flow))
#define DP_CSC_0(flow)			IPU_DP_REG(0x0054 + (flow))
#define DP_CSC_1(flow)			IPU_DP_REG(0x0058 + (flow))

enum {
	IPU_CONF_CSI0_EN = 0x00000001,
	IPU_CONF_CSI1_EN = 0x00000002,
	IPU_CONF_IC_EN = 0x00000004,
	IPU_CONF_ROT_EN = 0x00000008,
	IPU_CONF_ISP_EN = 0x00000010,
	IPU_CONF_DP_EN = 0x00000020,
	IPU_CONF_DI0_EN = 0x00000040,
	IPU_CONF_DI1_EN = 0x00000080,
	IPU_CONF_DMFC_EN = 0x00000400,
	IPU_CONF_SMFC_EN = 0x00000100,
	IPU_CONF_DC_EN = 0x00000200,
	IPU_CONF_VDI_EN = 0x00001000,
	IPU_CONF_IDMAC_DIS = 0x00400000,
	IPU_CONF_IC_DMFC_SEL = 0x02000000,
	IPU_CONF_IC_DMFC_SYNC = 0x04000000,
	IPU_CONF_VDI_DMFC_SYNC = 0x08000000,
	IPU_CONF_CSI0_DATA_SOURCE = 0x10000000,
	IPU_CONF_CSI0_DATA_SOURCE_OFFSET = 28,
	IPU_CONF_CSI1_DATA_SOURCE = 0x20000000,
	IPU_CONF_IC_INPUT = 0x40000000,
	IPU_CONF_CSI_SEL = 0x80000000,

	DI0_COUNTER_RELEASE = 0x01000000,
	DI1_COUNTER_RELEASE = 0x02000000,

	DI_DW_GEN_ACCESS_SIZE_OFFSET = 24,
	DI_DW_GEN_COMPONENT_SIZE_OFFSET = 16,

	DI_GEN_DI_CLK_EXT = 0x100000,
	DI_GEN_POLARITY_DISP_CLK = 0x00020000,
	DI_GEN_POLARITY_1 = 0x00000001,
	DI_GEN_POLARITY_2 = 0x00000002,
	DI_GEN_POLARITY_3 = 0x00000004,
	DI_GEN_POLARITY_4 = 0x00000008,
	DI_GEN_POLARITY_5 = 0x00000010,
	DI_GEN_POLARITY_6 = 0x00000020,
	DI_GEN_POLARITY_7 = 0x00000040,
	DI_GEN_POLARITY_8 = 0x00000080,

	DI_POL_DRDY_DATA_POLARITY = 0x00000080,
	DI_POL_DRDY_POLARITY_15 = 0x00000010,

	DI_VSYNC_SEL_OFFSET = 13,

	DC_WR_CH_CONF_FIELD_MODE = 0x00000200,
	DC_WR_CH_CONF_PROG_TYPE_OFFSET = 5,
	DC_WR_CH_CONF_PROG_TYPE_MASK = 0x000000E0,
	DC_WR_CH_CONF_PROG_DI_ID = 0x00000004,
	DC_WR_CH_CONF_PROG_DISP_ID_OFFSET = 3,
	DC_WR_CH_CONF_PROG_DISP_ID_MASK = 0x00000018,

	DC_UGDE_0_ODD_EN = 0x02000000,
	DC_UGDE_0_ID_CODED_MASK = 0x00000007,
	DC_UGDE_0_ID_CODED_OFFSET = 0,
	DC_UGDE_0_EV_PRIORITY_MASK = 0x00000078,
	DC_UGDE_0_EV_PRIORITY_OFFSET = 3,

	DP_COM_CONF_FG_EN = 0x00000001,
	DP_COM_CONF_GWSEL = 0x00000002,
	DP_COM_CONF_GWAM = 0x00000004,
	DP_COM_CONF_GWCKE = 0x00000008,
	DP_COM_CONF_CSC_DEF_MASK = 0x00000300,
	DP_COM_CONF_CSC_DEF_OFFSET = 8,
	DP_COM_CONF_CSC_DEF_FG = 0x00000300,
	DP_COM_CONF_CSC_DEF_BG = 0x00000200,
	DP_COM_CONF_CSC_DEF_BOTH = 0x00000100,
	DP_COM_CONF_GAMMA_EN = 0x00001000,
	DP_COM_CONF_GAMMA_YUV_EN = 0x00002000,

	DI_SER_CONF_LLA_SER_ACCESS = 0x00000020,
	DI_SER_CONF_SERIAL_CLK_POL = 0x00000010,
	DI_SER_CONF_SERIAL_DATA_POL = 0x00000008,
	DI_SER_CONF_SERIAL_RS_POL = 0x00000004,
	DI_SER_CONF_SERIAL_CS_POL = 0x00000002,
	DI_SER_CONF_WAIT4SERIAL = 0x00000001,
};

enum di_pins {
	DI_PIN11 = 0,
	DI_PIN12 = 1,
	DI_PIN13 = 2,
	DI_PIN14 = 3,
	DI_PIN15 = 4,
	DI_PIN16 = 5,
	DI_PIN17 = 6,
	DI_PIN_CS = 7,

	DI_PIN_SER_CLK = 0,
	DI_PIN_SER_RS = 1,
};

enum di_sync_wave {
	DI_SYNC_NONE = -1,
	DI_SYNC_CLK = 0,
	DI_SYNC_INT_HSYNC = 1,
	DI_SYNC_HSYNC = 2,
	DI_SYNC_VSYNC = 3,
	DI_SYNC_DE = 5,
};

/* DC template opcodes */
#define WROD(lf)		(0x18 | (lf << 1))
#define WRG	        	(0x01)

/* IPU Driver channels definitions.	*/
/* Note these are different from IDMA channels */
#define IPU_MAX_CH	32
#define _MAKE_CHAN(num, v_in, g_in, a_in, out) \
	((num << 24) | (v_in << 18) | (g_in << 12) | (a_in << 6) | out)
#define _MAKE_ALT_CHAN(ch)		(ch | (IPU_MAX_CH << 24))
#define IPU_CHAN_ID(ch)			(ch >> 24)
#define IPU_CHAN_ALT(ch)		(ch & 0x02000000)
#define IPU_CHAN_ALPHA_IN_DMA(ch)	((uint32_t) (ch >> 6) & 0x3F)
#define IPU_CHAN_GRAPH_IN_DMA(ch)	((uint32_t) (ch >> 12) & 0x3F)
#define IPU_CHAN_VIDEO_IN_DMA(ch)	((uint32_t) (ch >> 18) & 0x3F)
#define IPU_CHAN_OUT_DMA(ch)		((uint32_t) (ch & 0x3F))
#define NO_DMA 0x3F
#define ALT	1
/*!
 * Enumeration of IPU logical channels. An IPU logical channel is defined as a
 * combination of an input (memory to IPU), output (IPU to memory), and/or
 * secondary input IDMA channels and in some cases an Image Converter task.
 * Some channels consist of only an input or output.
 */
typedef enum {
	CHAN_NONE = -1,
	MEM_ROT_ENC_MEM = _MAKE_CHAN(1, 45, NO_DMA, NO_DMA, 48),
	MEM_ROT_VF_MEM = _MAKE_CHAN(2, 46, NO_DMA, NO_DMA, 49),
	MEM_ROT_PP_MEM = _MAKE_CHAN(3, 47, NO_DMA, NO_DMA, 50),

	MEM_PRP_ENC_MEM = _MAKE_CHAN(4, 12, 14, 17, 20),
	MEM_PRP_VF_MEM = _MAKE_CHAN(5, 12, 14, 17, 21),
	MEM_PP_MEM = _MAKE_CHAN(6, 11, 15, 18, 22),

	MEM_DC_SYNC = _MAKE_CHAN(7, 28, NO_DMA, NO_DMA, NO_DMA),
	MEM_DC_ASYNC = _MAKE_CHAN(8, 41, NO_DMA, NO_DMA, NO_DMA),
	MEM_BG_SYNC = _MAKE_CHAN(9, 23, NO_DMA, NO_DMA, NO_DMA),
	MEM_FG_SYNC = _MAKE_CHAN(10, 27, NO_DMA, 31, NO_DMA),

	MEM_BG_ASYNC0 = _MAKE_CHAN(11, 24, NO_DMA, 52, NO_DMA),
	MEM_FG_ASYNC0 = _MAKE_CHAN(12, 29, NO_DMA, 33, NO_DMA),
	MEM_BG_ASYNC1 = _MAKE_ALT_CHAN(MEM_BG_ASYNC0),
	MEM_FG_ASYNC1 = _MAKE_ALT_CHAN(MEM_FG_ASYNC0),

	DIRECT_ASYNC0 = _MAKE_CHAN(13, NO_DMA, NO_DMA, NO_DMA, NO_DMA),
	DIRECT_ASYNC1 = _MAKE_CHAN(14, NO_DMA, NO_DMA, NO_DMA, NO_DMA),

	CSI_MEM0 = _MAKE_CHAN(15, NO_DMA, NO_DMA, NO_DMA, 0),
	CSI_MEM1 = _MAKE_CHAN(16, NO_DMA, NO_DMA, NO_DMA, 1),
	CSI_MEM2 = _MAKE_CHAN(17, NO_DMA, NO_DMA, NO_DMA, 2),
	CSI_MEM3 = _MAKE_CHAN(18, NO_DMA, NO_DMA, NO_DMA, 3),

	CSI_MEM = CSI_MEM0,

	CSI_PRP_ENC_MEM = _MAKE_CHAN(19, NO_DMA, NO_DMA, NO_DMA, 20),
	CSI_PRP_VF_MEM = _MAKE_CHAN(20, NO_DMA, NO_DMA, NO_DMA, 21),

	/* for vdi mem->vdi->ic->mem , add graphics plane and alpha*/
	MEM_VDI_PRP_VF_MEM_P = _MAKE_CHAN(21, 8, 14, 17, 21),
	MEM_VDI_PRP_VF_MEM = _MAKE_CHAN(22, 9, 14, 17, 21),
	MEM_VDI_PRP_VF_MEM_N = _MAKE_CHAN(23, 10, 14, 17, 21),

	/* for vdi mem->vdi->mem */
	MEM_VDI_MEM_P = _MAKE_CHAN(24, 8, NO_DMA, NO_DMA, 5),
	MEM_VDI_MEM = _MAKE_CHAN(25, 9, NO_DMA, NO_DMA, 5),
	MEM_VDI_MEM_N = _MAKE_CHAN(26, 10, NO_DMA, NO_DMA, 5),

	/* fake channel for vdoa to link with IPU */
	MEM_VDOA_MEM =  _MAKE_CHAN(27, NO_DMA, NO_DMA, NO_DMA, NO_DMA),

	MEM_PP_ADC = CHAN_NONE,
	ADC_SYS2 = CHAN_NONE,

} ipu_channel_t;

/*!
 * Enumeration of types of buffers for a logical channel.
 */
typedef enum {
	IPU_OUTPUT_BUFFER = 0,	/*!< Buffer for output from IPU */
	IPU_ALPHA_IN_BUFFER = 1,	/*!< Buffer for input to IPU */
	IPU_GRAPH_IN_BUFFER = 2,	/*!< Buffer for input to IPU */
	IPU_VIDEO_IN_BUFFER = 3,	/*!< Buffer for input to IPU */
	IPU_INPUT_BUFFER = IPU_VIDEO_IN_BUFFER,
	IPU_SEC_INPUT_BUFFER = IPU_GRAPH_IN_BUFFER,
} ipu_buffer_t;

#define IPU_PANEL_SERIAL		1
#define IPU_PANEL_PARALLEL		2

/*!
 * Enumeration of ADC channel operation mode.
 */
typedef enum {
	Disable,
	WriteTemplateNonSeq,
	ReadTemplateNonSeq,
	WriteTemplateUnCon,
	ReadTemplateUnCon,
	WriteDataWithRS,
	WriteDataWoRS,
	WriteCmd
} mcu_mode_t;

/*!
 * Enumeration of ADC channel addressing mode.
 */
typedef enum {
	FullWoBE,
	FullWithBE,
	XY
} display_addressing_t;

enum v4l2_field {
	V4L2_FIELD_ANY           = 0, /* driver can choose from none,
					 top, bottom, interlaced
					 depending on whatever it thinks
					 is approximate ... */
	V4L2_FIELD_NONE          = 1, /* this device has no fields ... */
	V4L2_FIELD_TOP           = 2, /* top field only */
	V4L2_FIELD_BOTTOM        = 3, /* bottom field only */
	V4L2_FIELD_INTERLACED    = 4, /* both fields interlaced */
	V4L2_FIELD_SEQ_TB        = 5, /* both fields sequential into one
					 buffer, top-bottom order */
	V4L2_FIELD_SEQ_BT        = 6, /* same as above + bottom-top order */
	V4L2_FIELD_ALTERNATE     = 7, /* both fields alternating into
					 separate buffers */
	V4L2_FIELD_INTERLACED_TB = 8, /* both fields interlaced, top field
					 first and the top field is
					 transmitted first */
	V4L2_FIELD_INTERLACED_BT = 9, /* both fields interlaced, top field
					 first and the bottom field is
					 transmitted first */
};


/*!
 * Union of initialization parameters for a logical channel.
 */
typedef union {
	struct {
		uint32_t csi;
		uint32_t mipi_id;
		uint32_t mipi_vc;
		bool mipi_en;
		bool interlaced;
	} csi_mem;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
		uint32_t outh_resize_ratio;
		uint32_t outv_resize_ratio;
		uint32_t csi;
		uint32_t mipi_id;
		uint32_t mipi_vc;
		bool mipi_en;
	} csi_prp_enc_mem;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
		uint32_t outh_resize_ratio;
		uint32_t outv_resize_ratio;
	} mem_prp_enc_mem;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
	} mem_rot_enc_mem;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
		uint32_t outh_resize_ratio;
		uint32_t outv_resize_ratio;
		bool graphics_combine_en;
		bool global_alpha_en;
		bool key_color_en;
		uint32_t in_g_pixel_fmt;
		uint8_t alpha;
		uint32_t key_color;
		bool alpha_chan_en;
		ipu_motion_sel motion_sel;
		enum v4l2_field field_fmt;
		uint32_t csi;
		uint32_t mipi_id;
		uint32_t mipi_vc;
		bool mipi_en;
	} csi_prp_vf_mem;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
		bool graphics_combine_en;
		bool global_alpha_en;
		bool key_color_en;
		display_port_t disp;
		uint32_t out_left;
		uint32_t out_top;
	} csi_prp_vf_adc;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
		uint32_t outh_resize_ratio;
		uint32_t outv_resize_ratio;
		bool graphics_combine_en;
		bool global_alpha_en;
		bool key_color_en;
		uint32_t in_g_pixel_fmt;
		uint8_t alpha;
		uint32_t key_color;
		bool alpha_chan_en;
		ipu_motion_sel motion_sel;
		enum v4l2_field field_fmt;
	} mem_prp_vf_mem;
	struct {
		uint32_t temp;
	} mem_prp_vf_adc;
	struct {
		uint32_t temp;
	} mem_rot_vf_mem;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
		uint32_t outh_resize_ratio;
		uint32_t outv_resize_ratio;
		bool graphics_combine_en;
		bool global_alpha_en;
		bool key_color_en;
		uint32_t in_g_pixel_fmt;
		uint8_t alpha;
		uint32_t key_color;
		bool alpha_chan_en;
	} mem_pp_mem;
	struct {
		uint32_t temp;
	} mem_rot_mem;
	struct {
		uint32_t in_width;
		uint32_t in_height;
		uint32_t in_pixel_fmt;
		uint32_t out_width;
		uint32_t out_height;
		uint32_t out_pixel_fmt;
		bool graphics_combine_en;
		bool global_alpha_en;
		bool key_color_en;
		display_port_t disp;
		uint32_t out_left;
		uint32_t out_top;
	} mem_pp_adc;
	struct {
		uint32_t di;
		bool interlaced;
		uint32_t in_pixel_fmt;
		uint32_t out_pixel_fmt;
	} mem_dc_sync;
	struct {
		uint32_t temp;
	} mem_sdc_fg;
	struct {
		uint32_t di;
		bool interlaced;
		uint32_t in_pixel_fmt;
		uint32_t out_pixel_fmt;
		bool alpha_chan_en;
	} mem_dp_bg_sync;
	struct {
		uint32_t temp;
	} mem_sdc_bg;
	struct {
		uint32_t di;
		bool interlaced;
		uint32_t in_pixel_fmt;
		uint32_t out_pixel_fmt;
		bool alpha_chan_en;
	} mem_dp_fg_sync;
	struct {
		uint32_t di;
	} direct_async;
	struct {
		display_port_t disp;
		mcu_mode_t ch_mode;
		uint32_t out_left;
		uint32_t out_top;
	} adc_sys1;
	struct {
		display_port_t disp;
		mcu_mode_t ch_mode;
		uint32_t out_left;
		uint32_t out_top;
	} adc_sys2;
} ipu_channel_params_t;

#define IPU_IRQF_NONE		0x00000000
#define IPU_IRQF_ONESHOT	0x00000001

struct ipu_soc;

extern int ipu_probe();

struct ipu_soc *ipu_get();
void ipu_uninit_channel(struct ipu_soc *ipu, ipu_channel_t channel);
void ipu_disable_hsp_clk(struct ipu_soc *ipu);

extern int32_t ipu_init_channel_buffer(struct ipu_soc *ipu, ipu_channel_t channel,
				ipu_buffer_t type,
				uint32_t pixel_fmt,
				uint16_t width, uint16_t height,
				uint32_t stride,
				dma_addr_t phyaddr_0);

int32_t ipu_init_sync_panel(struct ipu_soc *ipu, int disp,
			    struct fb_info *fbi,
			    uint32_t pixel_fmt);

extern  int32_t ipu_init_channel(struct ipu_soc *ipu, ipu_channel_t channel, ipu_channel_params_t *params);

extern int32_t ipu_enable_channel(struct ipu_soc *ipu, ipu_channel_t channel);
extern int32_t ipu_disable_channel(struct ipu_soc *ipu, ipu_channel_t channel, bool wait_for_stop);

int ipu_request_irq(struct ipu_soc *ipu, uint32_t irq,
		    /* irq_return_t(*handler) (int, void *), */
		    uint32_t irq_flags, const char *devname, void *dev_id);

void ipu_uninit_sync_panel(struct ipu_soc *ipu, int disp);
#endif /* SRC_DRIVERS_GPU_IPU_V3_IPU_REGS_H_ */
