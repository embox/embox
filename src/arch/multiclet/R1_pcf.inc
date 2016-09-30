;;;; Constant list (assembler)

;;;;;;;;;;;;;;;;;;;;;;
;; GPIO constant	;;
;;;;;;;;;;;;;;;;;;;;;;

;;GPIOA

.alias GPIOA_IN		0xC00F0000
.alias GPIOA_OUT	0xC00F0004
.alias GPIOA_DIR	0xC00F0008
.alias GPIOA_MSK	0xC00F000C
.alias GPIOA_POL	0xC00F0010
.alias GPIOA_EDG	0xC00F0014
.alias GPIOA_BPS	0xC00F0018

;;GPIOB

.alias GPIOB_IN		0xC00F0100
.alias GPIOB_OUT	0xC00F0104
.alias GPIOB_DIR	0xC00F0108
.alias GPIOB_MSK	0xC00F010C
.alias GPIOB_POL	0xC00F0110
.alias GPIOB_EDG	0xC00F0114
.alias GPIOB_BPS	0xC00F0118

;;GPIOC

.alias GPIOC_IN		0xC00F0200
.alias GPIOC_OUT	0xC00F0204
.alias GPIOC_DIR	0xC00F0208
.alias GPIOC_MSK	0xC00F020C
.alias GPIOC_POL	0xC00F0210
.alias GPIOC_EDG	0xC00F0214
.alias GPIOC_BPS	0xC00F0218

;;GPIOD

.alias GPIOD_IN		0xC01F0300
.alias GPIOD_OUT	0xC01F0304
.alias GPIOD_DIR	0xC01F0308
.alias GPIOD_MSK	0xC01F030C
.alias GPIOD_POL	0xC01F0310
.alias GPIOD_EDG	0xC01F0314
.alias GPIOD_BPS	0xC01F0318

;;GPIOE

.alias GPIOE_IN		0xC01F0400
.alias GPIOE_OUT	0xC01F0404
.alias GPIOE_DIR	0xC01F0408
.alias GPIOE_MSK	0xC01F040C
.alias GPIOE_POL	0xC01F0410
.alias GPIOE_EDG	0xC01F0414
.alias GPIOE_BPS	0xC01F0418

;;GPIOF

.alias GPIOF_IN		0xC01F0500
.alias GPIOF_OUT	0xC01F0504
.alias GPIOF_DIR	0xC01F0508
.alias GPIOF_MSK	0xC01F050C
.alias GPIOF_POL	0xC01F0510
.alias GPIOF_EDG	0xC01F0514
.alias GPIOF_BPS	0xC01F0518


;;;;;;;;;;;;;;;;;;;;;;
;; Timer constant	;;
;;;;;;;;;;;;;;;;;;;;;;

;;Timer0

.alias TIM0_PSCVAL	0xC0010000
.alias TIM0_PSCPER 	0xC0010004
.alias TIM0_PSC 	0xC0010004 ;no use in the future
.alias TIM0_CNTVAL0	0xC0010010
.alias TIM0_CNT0 	0xC0010014 ;no use in the future
.alias TIM0_CNTPER0	0xC0010014
.alias TIM0_CR0   	0xC0010018
.alias TIM0_CNTVAL1	0xC0010020
.alias TIM0_CNT1 	0xC0010024
.alias TIM0_CNTPER1	0xC0010024
.alias TIM0_CR1   	0xC0010028
.alias TIM0_CNTVAL2	0xC0010030
.alias TIM0_CNT2 	0xC0010034
.alias TIM0_CNTPER2	0xC0010034
.alias TIM0_CR2   	0xC0010038
.alias TIM0_CNTVAL3	0xC0010040
.alias TIM0_CNT3 	0xC0010044
.alias TIM0_CNTPER3	0xC0010044
.alias TIM0_CR3   	0xC0010048

;;Timer1

.alias TIM1_PSCVAL	0xC0010100
.alias TIM1_PSC 	0xC0010104 ;no use in the future
.alias TIM1_PSCPER0	0xC0010104
.alias TIM1_CNTVAL0	0xC0010110
.alias TIM1_CNTPER0	0xC0010114
.alias TIM1_CNT0 	0xC0010114
.alias TIM1_CR0   	0xC0010118
.alias TIM1_CNTVAL1	0xC0010120
.alias TIM1_CNT1 	0xC0010124
.alias TIM1_CNTPER1	0xC0010124
.alias TIM1_CR1   	0xC0010128

;;Timer2

.alias TIM2_PSCVAL	0xC0110000
.alias TIM2_PSC 	0xC0110004 ;no use in the future
.alias TIM2_PSCPER 	0xC0110004
.alias TIM2_CNTVAL0	0xC0110010
.alias TIM2_CNTPER0	0xC0110014
.alias TIM2_CNT0 	0xC0110014
.alias TIM2_CR0   	0xC0110018
.alias TIM2_CNTVAL1	0xC0110020
.alias TIM2_CNTPER1	0xC0110024
.alias TIM2_CNT1 	0xC0110024
.alias TIM2_CR1   	0xC0110028
.alias TIM2_CNTVAL2	0xC0110030
.alias TIM2_CNTPER2	0xC0110034
.alias TIM2_CNT2 	0xC0110034
.alias TIM2_CR2   	0xC0110038
.alias TIM2_CNTVAL3	0xC0110040
.alias TIM2_CNTPER3	0xC0110044
.alias TIM2_CNT3 	0xC0110044
.alias TIM2_CR3   	0xC0110048

;;Timer3

.alias TIM3_PSCVAL	0xC0110100
.alias TIM3_PSC 	0xC0110104 ;no use in the future
.alias TIM3_PSCPER 	0xC0110104
.alias TIM3_CNTVAL0	0xC0110110
.alias TIM3_CNTPER0	0xC0110114
.alias TIM3_CNT0 	0xC0110114
.alias TIM3_CR0   	0xC0110118
.alias TIM3_CNTVAL1	0xC0110120
.alias TIM3_CNTPER1	0xC0110124
.alias TIM3_CNT1 	0xC0110124
.alias TIM3_CR1   	0xC0110128

;;;;;;;;;;;;;;;;;;;;;;
;; UART constant	;;
;;;;;;;;;;;;;;;;;;;;;;

;;UART0

.alias UART0_CR 	0xC0000108
.alias UART0_ST 	0xC0000104
.alias UART0_BDR	0xC000010C
.alias UART0_DATA 	0xC0000100

;;UART1

.alias UART1_CR 	0xC0000208
.alias UART1_ST 	0xC0000204
.alias UART1_BDR	0xC000020C
.alias UART1_DATA 	0xC0000200

;;UART2

.alias UART2_CR 	0xC0100108
.alias UART2_ST 	0xC0100104
.alias UART2_BDR	0xC010010C
.alias UART2_DATA 	0xC0100100

;;UART3

.alias UART3_CR 	0xC0100208
.alias UART3_ST 	0xC0100204
.alias UART3_BDR	0xC010020C
.alias UART3_DATA 	0xC0100200

;;;;;;;;;;;;;;;;;;
;; USB constant	;;
;;;;;;;;;;;;;;;;;;

;;USB0

.alias USB_En0_CR_OUT  		    0xFFFF0000
.alias USB_En0_DMA_CR_OUT	    0xFFFF0004
.alias USB_En0_DMA_ADDR_OUT		0xFFFF0008
.alias USB_En0_ST_OUT  		    0xFFFF000C
.alias USB_En1_CR_OUT  		    0xFFFF0010
.alias USB_En1_DMA_CR_OUT	    0xFFFF0014
.alias USB_En1_DMA_ADDR_OUT		0xFFFF0018
.alias USB_En1_ST_OUT  		    0xFFFF001C
.alias USB_En2_CR_OUT  		    0xFFFF0020
.alias USB_En2_DMA_CR_OUT	    0xFFFF0024
.alias USB_En2_DMA_ADDR_OUT		0xFFFF0028
.alias USB_En2_ST_OUT  		    0xFFFF002C
.alias USB_En3_CR_OUT  		    0xFFFF0030
.alias USB_En3_DMA_CR_OUT	    0xFFFF0034
.alias USB_En3_DMA_ADDR_OUT		0xFFFF0038
.alias USB_En3_ST_OUT  		    0xFFFF003C

.alias USB_En0_CR_IN  		    0xFFFF0100
.alias USB_En0_DMA_CR_IN	    0xFFFF0104
.alias USB_En0_DMA_ADDR_IN		0xFFFF0108
.alias USB_En0_ST_IN  		    0xFFFF010C
.alias USB_En1_CR_IN  		    0xFFFF0110
.alias USB_En1_DMA_CR_IN	    0xFFFF0114
.alias USB_En1_DMA_ADDR_IN		0xFFFF0118
.alias USB_En1_ST_IN  		    0xFFFF011C
.alias USB_En2_CR_IN  		    0xFFFF0120
.alias USB_En2_DMA_CR_IN	    0xFFFF0124
.alias USB_En2_DMA_ADDR_IN		0xFFFF0128
.alias USB_En2_ST_IN  		    0xFFFF012C
.alias USB_En3_CR_IN  		    0xFFFF0130
.alias USB_En3_DMA_CR_IN	    0xFFFF0134
.alias USB_En3_DMA_ADDR_IN		0xFFFF0138
.alias USB_En3_ST_IN  		    0xFFFF013C

.alias USB_CR       		0xFFFF0200
.alias USB_ST       		0xFFFF0204

;;;;;;;;;;;;;;;;;;
;; I2C constant	;;
;;;;;;;;;;;;;;;;;;

;I2C0 master

.alias I2C0_CPS			0xC0001000 ;no use in the future
.alias I2C0_PSC			0xC0001000
.alias I2C0_CR			0xC0001004
.alias I2C0_RDATA		0xC0001008 ;no use in the future
.alias I2C0_RX			0xC0001008 ;no use in the future
.alias I2C0_TDATA		0xC0001008 ;no use in the future
.alias I2C0_TX			0xC0001008 ;no use in the future
.alias I2C0_DATA		0xC0001008
.alias I2C0_ST			0xC000100C ;no use in the future
.alias I2C0_CMD			0xC000100C ;no use in the future
.alias I2C0_STCMD		0xC000100C

;I2C1 slave

.alias I2C1_SAD			0xC0001100 ;no use in the future
.alias I2C1_ADDR		0xC0001100
.alias I2C1_CR			0xC0001104
.alias I2C1_ST			0xC0001108
.alias I2C1_MSK			0xC000110C
.alias I2C1_RDATA		0xC0001110 ;no use in the future
.alias I2C1_RX			0xC0001110
.alias I2C1_TDATA		0xC0001114 ;no use in the future
.alias I2C1_TX			0xC0001114

;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Ethernet constant	;;
;;;;;;;;;;;;;;;;;;;;;;;;;;

.alias ETH_CR			0xC0005000
.alias ETH_ST			0xC0005004
.alias ETH_MACMSB		0xC0005008
.alias ETH_MACLSB		0xC000500C
.alias ETH_MDIOST		0xC0005010
.alias ETH_TDP			0xC0005014
.alias ETH_RDP			0xC0005018

;;;;;;;;;;;;;;;;;;
;; SPI constant	;;
;;;;;;;;;;;;;;;;;;

;SPI0

.alias SPI0_CFG			0xC0104000
.alias SPI0_CR			0xC0104020
.alias SPI0_ST			0xC0104024
.alias SPI0_MSK			0xC0104028
.alias SPI0_CMD			0xC010402C
.alias SPI0_TX			0xC0104030
.alias SPI0_RX			0xC0104034
.alias SPI0_SS			0xC0104038

;SPI1

.alias SPI1_CFG			0xC0108000
.alias SPI1_CR			0xC0108020
.alias SPI1_ST			0xC0108024
.alias SPI1_MSK			0xC0108028
.alias SPI1_CMD			0xC010802C
.alias SPI1_TX			0xC0108030
.alias SPI1_RX			0xC0108034
.alias SPI1_SS			0xC0108038

;;;;;;;;;;;;;;;;;;
;; WDT constant	;;
;;;;;;;;;;;;;;;;;;

.alias WDT_CNT			0xC00E0000
.alias WDT_KEY			0xC00E0004
.alias WDT_ST			0xC00E000C

;;;;;;;;;;;;;;;;;;;;
;; MCTRL constant ;;
;;;;;;;;;;;;;;;;;;;;

.alias MCFG1			0xC00EE000
.alias MCFG2			0xC00EE004
.alias MCFG3			0xC00EE008
.alias MCFG4			0xC00EE00C

;;;;;;;;;;;;;;;;;;
;; RTC constant	;;
;;;;;;;;;;;;;;;;;;

.alias RTC_TIME			0xC001F000
.alias RTC_DATE			0xC001F004
.alias RTC_CR			0xC001F008
.alias RTC_PSC			0xC001F00C
.alias RTC_ST			0xC001F010
.alias RTC_ALARMA		0xC001F014
.alias RTC_ALARMB		0xC001F018

;;;;;;;;;;;;;;;;;;
;; I2S constant	;;
;;;;;;;;;;;;;;;;;;

.alias I2S_CFG			0xC0002000
.alias I2S_MSK			0xC0002004
.alias I2S_INT			0xC0002008
.alias I2S_RX			0xC000200C

;;;;;;;;;;;;;;;;;;
;; PWM constant	;;
;;;;;;;;;;;;;;;;;;

.alias PWM_CR			0xC0112000
.alias PWM_INT			0xC0112004
.alias PWM_CNTVAL		0xC0112008
.alias PWM_PSC			0xC011200C
.alias PWM_CNT			0xC0112010
.alias PWM_CMPCH0		0xC0112020
.alias PWM_CMPCH1		0xC0112024
.alias PWM_CMPCH2		0xC0112028
.alias PWM_CMPCH3		0xC011202C

;;;;;;;;;;;;;;;;;;;
;; ADC0 constant ;;
;;;;;;;;;;;;;;;;;;;

.alias ADC0_CR			0xC01D0000
.alias ADC0_CH0			0xC01D0004
.alias ADC0_CH1			0xC01D0008
.alias ADC0_CH2			0xC01D000C
.alias ADC0_CH3			0xC01D0010

;;;;;;;;;;;;;;;;;;;
;; ADC1 constant ;;
;;;;;;;;;;;;;;;;;;;

.alias ADC1_CR			0xC01D0100
.alias ADC1_CH0			0xC01D0104
.alias ADC1_CH1			0xC01D0108
.alias ADC1_CH2			0xC01D010C
.alias ADC1_CH3			0xC01D0110

;;;;;;;;;;;;;;;;;;;
;; DAC0 constant ;;
;;;;;;;;;;;;;;;;;;;

.alias DAC0_PD			0xC01D1000
.alias DAC0_DATA		0xC01D1004

;;;;;;;;;;;;;;;;;;;
;; DAC1 constant ;;
;;;;;;;;;;;;;;;;;;;

.alias DAC1_PD			0xC01D1100
.alias DAC1_DATA		0xC01D1104
