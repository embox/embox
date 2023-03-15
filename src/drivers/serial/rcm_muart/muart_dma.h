/**
 * @file
 *
 * @date Jan 17, 2023
 * @uthor Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_RCM_MUART_MUART_DMA_H_
#define SRC_DRIVERS_SERIAL_RCM_MUART_MUART_DMA_H_

#include <stdint.h>

struct muart_dma_status {
  uint32_t suspend_done:    1; /*!<    DMA_GP */
  uint32_t cancel_done:     1; /*!<	  DMA_GP (cancel) */
  uint32_t int_desc:        1; /*!<	     Int */
  uint32_t bad_desc:        1; /*!<	    DMA-GP ( ownership  1) */
  uint32_t stop_desc:       1; /*!<	     Stop */
  uint32_t discard_desc:    1; /*!<	     AXI    */
  uint32_t waxi_err:        1; /*!<	     AXI    */
  uint32_t axi_err:         1; /*!<	     AXI  /  */
  uint32_t start_by_event:  1; /*!<	     */
  uint32_t ignore_event:    1; /*!<	,  DMA-GP,      DMA-GP */
  uint32_t :                22; /*!<  */
};

struct muart_dma_settings {
  uint32_t desc_type:       2; /*!<     10  long- (128 ) */
  uint32_t:                 2; /*!<  */
  uint32_t add_info:        1; /*!<   1 */
  uint32_t:                 11; /*!<	 */
  uint32_t desc_gap:        16; /*!< 	   */
};

struct muart_dma_state {
	uint32_t wr_fifo_full :1; /*!<	 FIFO     */
	uint32_t wr_fifo_empty :1; /*!<	 FIFO     */
	uint32_t rd_fifo_empty :1; /*!<	 FIFO     */
	uint32_t :1; /*!<	 */
	uint32_t ready_for_str :1; /*!<	    pitch-,     */
	uint32_t ready_for_mod :1; /*!<	   ,     */
	uint32_t :2; /*!<	 */
	uint32_t data_desc_cnt :2; /*!<	  ,       DMA */
	uint32_t :2; /*!<	 */
	uint32_t desc_cnt :3; /*!<	 ,       DMA */
	uint32_t :1; /*!<  */
	uint32_t discard_desc :1; /*!<	      . */
	uint32_t bad_flag :1; /*!<	 ,   DMA */
	uint32_t stop_flag :1; /*!<	    stop */
	uint32_t stop :1; /*!<	    */
	uint32_t data_send_permit :1; /*!<	        */
	uint32_t cancel :1; /*!<	  cancel */
	uint32_t suspend :1; /*!<	  suspend */
	uint32_t enable :1; /*!<	  DMA */
	uint32_t desc_is_writing :1; /*!<	    */
	uint32_t desc_is_mod :1; /*!<	    */
	uint32_t wr_state :1; /*!<	  : 1      */
	uint32_t rd_state :1; /*!<	  : 1      */
	uint32_t state :2; /*!<	 DMA: 0   ; 1   ; 2   ; 3        ,     */
	uint32_t :2; /*!<	 */
};


struct muart_dma_regs {
	uint32_t Enable; /*!< 00	/  0x100 */
	uint32_t Suspend; /*!< 00	   */
	uint32_t Cancel; /*!< 00	       0x108 */
	const uint32_t res0; /*!<  */
	struct muart_dma_settings Settings; /*!< 00	  0x110 */
	struct muart_dma_status IrqMask; /*!< 0x0	   0x114 */
	const struct muart_dma_status Status; /*!< 0x0	    0x118 */
	const uint32_t res1; /*!<  */
	uint32_t DescAddr; /*!< 00	   */
	const uint32_t res2; /*!<  */
	const uint32_t CurDescAddr; /*!< 0x0	   */
	const uint32_t CurAddr; /*!< 0x0	   */
	const struct muart_dma_state DMA_State; /*!< 0x0	   0x130 */
	const uint32_t res3[3]; /*!<  */
	uint32_t DESC_AXLEN; /*!< 0x3	 ARLEN  AWLEN    AXI      (      [3:0]) */
	uint32_t DESC_ACACHE; /*!< 0x3	 ARCACHE  AWCACHE    AXI      (      [3:0]) */
	uint32_t DESC_APROT; /*!< 0x2	 ARPROT  AWPROT    AXI      (      [2:0]) */
	uint32_t DESC_ALOCK; /*!< 0x0	 ARLOCK  AWLOCK    AXI      (      [1:0]) */
	const uint32_t DESC_RRESP; /*!< 00	 RRESP    AXI    (      [1:0]) */
	const uint32_t DESC_RAXI_ERR_ADDR; /*!<   00	 AXI-  ,     AXI */
	const uint32_t DESC_BRESP; /*!< 00	 BRESP    AXI    (      [1:0]) */
	const uint32_t DESC_WAXI_ERR_ADDR; /*!<   00	 AXI-    ,     AXI */
	uint32_t DESC_PERMUT; /*!< 0x76543210	         AXI */
	const uint32_t res4[7]; /*!<  */
	uint32_t MaxTransNum; /*!< MAX_TRANS_NUM	    (  15) */
	uint32_t ARLEN; /*!< 0F	 AWLEN    AXI (      [3:0]) */
	uint32_t ARCACHE; /*!< 0x3	 AWCACHE    AXI (      [3:0]) */
	uint32_t ARPROT; /*!< 0x2	 AWPROT    AXI (      [2:0]) */
	uint32_t ARLOCK; /*!< 0x0	 AWLOCK    AXI (      [1:0]) */
	const uint32_t RRESP; /*!< 00	 BRESP    AXI (      [1:0]) */
	const uint32_t RAXI_ERR_ADDR; /*!<  00	 AXI- ,     AXI */
	const uint32_t res5; /*!< 0x19C	-	-	-	 */
	const uint32_t RAXI_STATE; /*!< 0x0	   AXI */
	const uint32_t AVAILABLE_SPACE; /*!<  0x50	      */
	uint32_t PERMUTATION; /*!<  0x76543210	       AXI */
	const uint32_t res6[21]; /*!<  */
};


#endif /* SRC_DRIVERS_SERIAL_RCM_MUART_MUART_DMA_H_ */
