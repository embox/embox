/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#include <util/log.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/gadget.h>

#include <kernel/printk.h>
#include <kernel/irq.h>
#include <lib/libds/array.h>

#include <embox/unit.h>

#include <util/math.h>

#include <drivers/clk/niiet_rcu.h>

#include <config/board_config.h>

#include "k1921vg015_udc_priv.h"

EMBOX_UNIT_INIT(niiet_udc_init);

#define USB_IRQ_NUM       CONF_USB_IRQ

#define NIIET_UDC_IN_EP_MASK  ((1 << 1) | (1 << 2) | (1 << 3))
#define NIIET_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))

#define USB      ((struct niiet_usbd_regs *)(uintptr_t) CONF_USB_REGION_BASE)

static uint8_t __attribute__ ((aligned(1024))) dma_tmpbuf[1024]; // for max ISOC HIGHSPEED transaction

static inline void USBDev_ParseSetupPacket(struct usb_control_header *ctrl) {
	ctrl->bm_request_type = (uint8_t)(USB->CEP_SETUP1_0 & 0xff);
	ctrl->b_request = (uint8_t)((USB->CEP_SETUP1_0 >> 8) & 0xff);
	ctrl->w_value = USB->CEP_SETUP3_2;
	ctrl->w_index = USB->CEP_SETUP5_4;
	ctrl->w_length = USB->CEP_SETUP7_6;
}

static inline int USBDev_CEPSendResponse(uint32_t resp) {
	USB->CEP_CTRL_STAT = resp;

	return 0;
}

static inline int USBDev_SetAddress(uint8_t address) {
	USB->USBADDR = address;

	return 0;
}

static inline int USBDev_DMAOpStart(uint8_t *pbuf, uint32_t len, uint8_t epnum, uint32_t op) {
	uint32_t wr_to_ep = DMA_CTRL_STS_OP_WRITE & op;

	if (len > 1024) {
		return 1;
	}

	if (wr_to_ep != 0) {
		memcpy(dma_tmpbuf, pbuf, len);
	}

	USB->AHB_DMA_ADDR = (uint32_t)&dma_tmpbuf[0];
	USB->DMA_CNT = len;
	USB->DMA_CTRL_STS = op | epnum;

	while (0 != (USB->DMA_CTRL_STS & DMA_CTRL_STS_DMAEN)) {
    };

	if (wr_to_ep == 0) {
		memcpy(pbuf, &dma_tmpbuf[0], len);
	}

	return 0;
}

static inline int USBDev_GetNextCEPPacket(void) {
	uint32_t data_size;
    uint8_t *buf = NULL;

	data_size = USB->CEP_OUT_XFRCNT;

	if (data_size > 0) {
		USBDev_DMAOpStart((uint8_t *)buf,
		    data_size, 0, DMA_CTRL_STS_OP_READ | DMA_CTRL_STS_DMAEN);
	}

	return 0;
}

static inline int USBDev_ReadPacket(uint8_t *dst, uint32_t len, uint8_t epnum)
{
    if(len > 0) {
        USBDev_DMAOpStart(dst, len, epnum, DMA_CTRL_STS_OP_READ | DMA_CTRL_STS_DMAEN);
	}
    return 0;
}

static inline int USBDev_WritePacket(uint8_t *src, uint32_t len, uint8_t epnum) {
    if(len > 0) {
        USBDev_DMAOpStart(src, len, epnum, DMA_CTRL_STS_OP_WRITE | DMA_CTRL_STS_DMAEN);

#ifdef USE_EP4
        USB->USB_EP[3].RSP_SC = USB_USB_EP_RSP_SC_PKTEND_Msk;
#endif

#ifdef USE_EP1
        USB->USB_EP[3].RSP_SC = USB_USB_EP_RSP_SC_PKTEND_Msk;
#endif

    }

    return 0;
}

static inline int USBDev_PutNextCEPPacket(void) {
    uint32_t data_size = 0;
	void *buf = NULL;
#if 0
    if((USBDev_0.setup_pkt.xfer_len - USBDev_0.setup_pkt.xfer_count) > USB_DEVICE_MAX_EP0_SIZE )
    	data_size = USB_DEVICE_MAX_EP0_SIZE;
    else
    	data_size  = USBDev_0.setup_pkt.xfer_len - USBDev_0.setup_pkt.xfer_count;
#endif
    if(data_size == 0) {
    	USBDev_CEPSendResponse(CEP_CTRL_STAT_ZEROLEN);
	} else {
    	USBDev_DMAOpStart((uint8_t*)(buf),
    	                          data_size, 0, DMA_CTRL_STS_OP_WRITE | DMA_CTRL_STS_DMAEN);
    	USB->CEP_IN_XFRCNT = data_size;
    }

    return 0;
}

static inline int USBDev_EPSendData(uint8_t *pbuf, uint32_t len, uint8_t epnum) {
    if(epnum == 0) {
        if(len == 0) {
            USBDev_CEPSendResponse(CEP_CTRL_STAT_ZEROLEN);
            return 0;
        }

        USBDev_PutNextCEPPacket();
    }
    else {

    }

    return 0;
}
static inline void usb_control_header_show(struct usb_control_header *ctrl) {
	if (log_level_self() >= LOG_DEBUG) {
		printk("usb_control_header:\n");
		printk("\tbm_request_type: %x\n", ctrl->bm_request_type);
		printk("\tb_request: %x\n", ctrl->b_request);
		printk("\tw_value: %x\n", ctrl->w_value);
		printk("\tw_index: %x\n", ctrl->w_index);
		printk("\tw_index: %x\n", ctrl->w_length);
	}

}

/*----- cep handler ------- */
static void niiet_ep_ctrl_handler(struct niiet_udc *niiet_udc) {
	struct usb_control_header ctrl = {0};
    uint32_t irq_stat;

	irq_stat = USB->CEP_IRQ_STAT;
    irq_stat &= USB->CEP_IRQ_ENB;

	if (log_level_self() >= LOG_DEBUG) {
		printk("irq_ep_ctrl irq_stat (0x%x)\n", irq_stat);
	}

	    // usb ERROR
    if(irq_stat & CEP_IRQ_USBERR) {
        //discard data
		printk("irq_ep_ctrl CEP_IRQ_USBERR\n");
        //USB->CEP_CTRL_STAT = CEP_CTRL_STAT_CEPFLUSH | CEP_CTRL_STAT_STALL;
		USB->CEP_CTRL_STAT = CEP_CTRL_STAT_NAKCLEAR;
        //and set for ready receive new SETUP packet
        //TODO:
        USB->CEP_IRQ_STAT = irq_stat;
		return ;
    }

    // SETUP packet was received
    if(irq_stat & CEP_IRQ_SETUPPKT) {
		USB->CEP_IRQ_STAT = CEP_IRQ_SETUPPKT;

    	USBDev_ParseSetupPacket(&ctrl);
    	usb_control_header_show(&ctrl);
		USBDev_CEPSendResponse(CEP_CTRL_STAT_STALL);

    }

    // DATA packet received
    if(irq_stat & CEP_IRQ_DATAPKTREC) {
        USB->CEP_IRQ_STAT = CEP_IRQ_DATAPKTREC;
        USBDev_GetNextCEPPacket();
printk("irq_ep_ctrl CEP_IRQ_DATAPKTREC\n");
        if (0){
			USBDev_CEPSendResponse(CEP_CTRL_STAT_ZEROLEN);
				//USBDev_SetSetupStage(USBDev_SetupStage_Status);
        } else {
        	USBDev_CEPSendResponse(CEP_CTRL_STAT_STALL);
        	//USBDev_SetSetupStage(USBDev_SetupStage_Wait);
        }
    }

    // DATA packet transmitted
    if(irq_stat & CEP_IRQ_DATAPKTTR) {
        USB->CEP_IRQ_STAT = CEP_IRQ_DATAPKTTR;
		printk("irq_ep_ctrl CEP_IRQ_DATAPKTTR\n");
        if(0) {
        		//TODO: add callback TX data event
        		USBDev_CEPSendResponse(CEP_CTRL_STAT_ACK);
        		//USBDev_SetSetupStage(USBDev_SetupStage_Status);
        } else {
        		//TODO:
        		USBDev_PutNextCEPPacket();
        	}

        } 
        #if 0
        else {
        	USBDev_CEPSendResponse(CEP_CTRL_STAT_STALL);
        	//USBDev_SetSetupStage(USBDev_SetupStage_Wait);
        }
        #endif
    

    // end STATUS stage of SETUP xfer
    if(irq_stat & CEP_IRQ_STATCMPLN) {
    	USB->CEP_IRQ_STAT = CEP_IRQ_STATCMPLN;
		printk("irq_ep_ctrl CEP_IRQ_STATCMPLN\n");
		USBDev_SetAddress(0);
	}

    if((irq_stat & CEP_IRQ_SETUPTOKEN) != 0) {
        //TODO:
        USB->CEP_IRQ_STAT = CEP_IRQ_SETUPTOKEN;
    }

    if((irq_stat & CEP_IRQ_INTOKEN) != 0) {
        //TODO:
        USB->CEP_IRQ_STAT = CEP_IRQ_INTOKEN;
    }

    if((irq_stat & CEP_IRQ_OUTTOKEN) != 0) {
        //TODO:
        USB->CEP_IRQ_STAT = CEP_IRQ_OUTTOKEN;
    }
}

static irq_return_t niiet_usbd_irq_handler(unsigned int irq_nr, void *data) {
	uint32_t irq_stat;
	struct niiet_udc *niiet_udc = data;
	ipl_t ipl;

	irq_stat = USB->INTSTAT0;

	if (irq_stat == 0) {
		return 0;
	}

	if (log_level_self() >= LOG_DEBUG) {
		printk("usb: irq entry INTSTAT0 (0x%x)\n", irq_stat);
	}

	ipl = ipl_save();

	irq_stat &= USB->INTEN0;

	if (irq_stat & CEP_IRQ_SETUPPKT) {
		niiet_ep_ctrl_handler(niiet_udc);
	}

	if (irq_stat & INTEN0_USBBUSINTEN) {
		uint32_t usbirq_stat;

		usbirq_stat = USB->INTSTAT1;
		if (log_level_self() >= LOG_DEBUG) {
			printk("niiet_usb: INTEN0_USBBUSINTEN INTSTAT1(%x)\n", usbirq_stat);
	    }
		usbirq_stat &= USB->INTEN1;

		if (usbirq_stat & INTEN1_CLKUNSTBL) {
			USB->INTSTAT1 = INTEN1_CLKUNSTBL;
		}

		if (usbirq_stat & INTEN1_SOF) {
			//usb_framecnt = USB->FRAMECNT_bit.FRAME_COUNTER;
			USB->INTSTAT1 = INTEN1_SOF;
		}

		if (usbirq_stat & INTEN1_RESTATUS) {
            if (log_level_self() >= LOG_DEBUG) {
		        printk("niiet_usb: INTEN1_RESTATUS\n");
	        }
			// configure control endpoint here for setup transactions
			USB->CEP_START_ADDR = 0;
			USB->CEP_END_ADDR = 64 - 1; //TODO: set as max control endpoint buffer value
			// enable interrupts for CEP
			USB->CEP_IRQ_ENB = CEP_IRQ_SETUPPKT | CEP_IRQ_DATAPKTREC
			                   | CEP_IRQ_DATAPKTTR | CEP_IRQ_STATCMPLN
			                   | CEP_IRQ_USBERR | CEP_IRQ_SETUPTOKEN;

			USB->INTEN0 |= INTEN0_CEP_INTEN;

			USB->INTSTAT1 = INTEN1_RESTATUS; //clear bit

			USB->INTEN1 |= INTEN1_SOF;

			USB->INTEN0 &= 0x3;
			USB->USB_EP[0].USB_EP_CFG = 0;
			USB->USB_EP[1].USB_EP_CFG = 0;
			USB->USB_EP[2].USB_EP_CFG = 0;
			USB->USB_EP[3].USB_EP_CFG = 0;

			//hid_report_updated = 0;

			//USBDev_ChangeState(&USBDev_0, Default);
			//USBDev_0.setup_pkt.stage = USBDev_SetupStage_Wait;
		}

		if (usbirq_stat & INTEN1_RESUME) {
			//USBDev_RevertState(&USBDev_0);

			USB->INTSTAT1 = INTEN1_RESUME; //clear bit
		}

		if (usbirq_stat & INTEN1_SUSPEND) {
			//USBDev_ChangeState(&USBDev_0, Suspended);

			USB->INTSTAT1 = INTEN1_SUSPEND; //clear bit
		}
		
        if((usbirq_stat & INTEN1_DMACMPL) != 0) {
            USB->INTSTAT1 = INTEN1_DMACMPL; //clear bit
        }

		if (usbirq_stat & INTEN1_CLKUNSTBL) {
			USB->INTSTAT1 = INTEN1_CLKUNSTBL; //clear bit
			//USB->INTEN1_bit.CLKUNSTBL = 0; //disable this irq
		}
	}

	ipl_restore(ipl);

	return IRQ_HANDLED;
}

static inline void niiet_udc_pll_init() {
#if 0
	/* SYSCLK */
	USB->PLLUSBCFG0 &= ~(PLLUSBCFG0_PLLEN);
	USB->PLLUSBCFG3 |= PLLUSBCFG3_USBCLKSEL;

	for (volatile int i = 0; i < 10000; ++i) {}
#endif

    uint32_t timeout_counter = 0;
	USB->PLLUSBCFG0 =( 7 << PLLUSBCFG0_PD1B_Pos) |  //PD1B
					 ( 7 << PLLUSBCFG0_PD1A_Pos) |  //PD1A
					 ( 1 << PLLUSBCFG0_PD0B_Pos) |  //PD0B 120FPRE/(1+1) = 60FOUT
					 ( 7 << PLLUSBCFG0_PD0A_Pos) |  //PD0A 960FVCO/(1+7) = 120FPRE
					 ( 1 << PLLUSBCFG0_REFDIV_Pos) 	  |  //refdiv 16/1 = 16FREF
					 ( 0 << PLLUSBCFG0_FOUTEN_Pos)    |  //fouten
					 ( 0 << PLLUSBCFG0_DSMEN_Pos)     |  //dsmen
					 ( 0 << PLLUSBCFG0_DACEN_Pos)     |  //dacen
					 ( 0 << PLLUSBCFG0_BYP_Pos)       |  //bypass
					 ( 1 << PLLUSBCFG0_PLLEN_Pos);       //en
	USB->PLLUSBCFG1 = 0;          //FRAC = 0
	USB->PLLUSBCFG2 = 59;         //FBDIV 16FREF*60=960FVCO

	USB->PLLUSBCFG0 |= ( 1 << PLLUSBCFG0_FOUTEN_Pos); 	// Fout0 Enable

	timeout_counter = 1000;
	while(timeout_counter) timeout_counter--;

	while (USB->PLLUSBSTAT & PLLUSBCFG0_LOCK)
	{}; 								// wait lock signal

	USB->PLLUSBCFG0 |= (2 << PLLUSBCFG0_BYP_Pos) ; 		// Bypass for Fout1

	USB->PLLUSBCFG3 &= ~PLLUSBCFG3_USBCLKSEL; //0-PLLUSBClk (FOUT0); 1- SYSClk
}

static int niiet_udc_start(struct usb_udc *udc) {
	clk_enable(CONF_USB_CLK_DEF_USB);
	niiet_udc_pll_init();

	USB->PHY_PD = 0;

    USB->INTEN1 = /* INTEN1_SUSPEND | INTEN1_RESUME | */ INTEN1_RESTATUS;
    USB->INTEN0 = INTEN0_USBBUSINTEN;

	return 0;
}

static int niiet_udc_ep_queue(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {

	return 0;
}

static int niiet_udc_ep_stall(struct usb_gadget_ep *ep,
    const struct usb_control_header *ctrl)  {
	return 0;
}

static void niiet_udc_ep_enable(struct usb_gadget_ep *ep) {

}

static struct usb_udc_ops niiet_usb_udc_ops = {
	.uuo_udc_start = niiet_udc_start,
	.uuo_ep_queue = niiet_udc_ep_queue,
	.uuo_ep_stall = niiet_udc_ep_stall,
	.uuo_ep_enable = niiet_udc_ep_enable,
};

struct niiet_udc niiet_udc = {
    .udc =
        {
            .udc_name = "udc",
            .udc_ops = &niiet_usb_udc_ops,

            .udc_in_ep_mask = NIIET_UDC_IN_EP_MASK,
            .udc_out_ep_mask = NIIET_UDC_OUT_EP_MASK,

			.udc_ep0_max_size = USB_MAX_EP0_SIZE,
			.udc_ep_max_size = CONF_USB_MISC_EP_MAX_SIZE,
        },
};

static int niiet_udc_init(void) {
	int ret;

	ret = irq_attach(USB_IRQ_NUM, niiet_usbd_irq_handler, 0, &niiet_udc, "udc");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	usb_gadget_udc_register(&niiet_udc.udc);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ_NUM, niiet_usbd_irq_handler, &niiet_udc);