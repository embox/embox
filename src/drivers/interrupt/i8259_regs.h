/**
 * @file
 * @brief Definitions for i8952 interrupt controller.
 *
 * @date 22.12.10
 * @author Nikolay Korotky
 */

#ifndef I8259_REGS_H_
#define I8259_REGS_H_

#define PIC1            0x20   /* IO base address for master PIC */
#define PIC2            0xA0   /* IO base address for slave PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1 + 1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2 + 1)

/* Default location in the IDT at which we program the PICs */
#define PIC1_BASE             0x20
#define PIC2_BASE             0x28

/* ICW1 (Initialization Control Word) */
#define ICW_TEMPLATE          0x10
#define LEVL_TRIGGER          0x08
#define EDGE_TRIGGER          0x00
#define ADDR_INTRVL4          0x04
#define ADDR_INTRVL8          0x00
#define SINGLE__MODE          0x02
#define CASCADE_MODE          0x00
#define ICW4__NEEDED          0x01
#define NO_ICW4_NEED          0x00

/* ICW3 */
#define SLAVE_ON_IR0          0x01
#define SLAVE_ON_IR1          0x02
#define SLAVE_ON_IR2          0x04
#define SLAVE_ON_IR3          0x08
#define SLAVE_ON_IR4          0x10
#define SLAVE_ON_IR5          0x20
#define SLAVE_ON_IR6          0x40
#define SLAVE_ON_IR7          0x80

#define I_AM_SLAVE_0          0x00
#define I_AM_SLAVE_1          0x01
#define I_AM_SLAVE_2          0x02
#define I_AM_SLAVE_3          0x03
#define I_AM_SLAVE_4          0x04
#define I_AM_SLAVE_5          0x05
#define I_AM_SLAVE_6          0x06
#define I_AM_SLAVE_7          0x07

/* ICW4 */
#define SNF_MODE_ENA          0x10
#define SNF_MODE_DIS          0x00
#define BUFFERD_MODE          0x08
#define NONBUFD_MODE          0x00
#define AUTO_EOI_MOD          0x02
#define NRML_EOI_MOD          0x00
#define I8086_EMM_MOD         0x01
#define SET_MCS_MODE          0x00

/* OCW1 (Operation Control Word) */
#define PICM_MASK             0xFF
#define PICS_MASK             0xFF

/* OCW2 */
#define NON_SPEC_EOI          0x20
#define SPECIFIC_EOI          0x60
#define ROT_NON_SPEC          0xa0
#define SET_ROT_AEOI          0x80
#define RSET_ROTAEOI          0x00
#define ROT_SPEC_EOI          0xe0
#define SET_PRIORITY          0xc0
#define NO_OPERATION          0x40
#define SEND_EOI_IR0          0x00
#define SEND_EOI_IR1          0x01
#define SEND_EOI_IR2          0x02
#define SEND_EOI_IR3          0x03
#define SEND_EOI_IR4          0x04
#define SEND_EOI_IR5          0x05
#define SEND_EOI_IR6          0x06
#define SEND_EOI_IR7          0x07

/* OCW3 */
#define OCW_TEMPLATE          0x08
#define SPECIAL_MASK          0x40
#define MASK_MDE_SET          0x20
#define MASK_MDE_RST          0x00
#define POLL_COMMAND          0x04
#define NO_POLL_CMND          0x00
#define READ_NEXT_RD          0x02
#define READ_IR_ONRD          0x00
#define READ_IS_ONRD          0x01

/**
 * Standard PIC initialization values
 */
#define PIC1_ICW1       (ICW_TEMPLATE | EDGE_TRIGGER | ADDR_INTRVL8 \
                         | CASCADE_MODE | ICW4__NEEDED)
#define PIC1_ICW3       (SLAVE_ON_IR2)
#define PIC1_ICW4       (SNF_MODE_DIS | NONBUFD_MODE | NRML_EOI_MOD \
                         | I8086_EMM_MOD)

#define PIC2_ICW1       (ICW_TEMPLATE | EDGE_TRIGGER | ADDR_INTRVL8 \
                         | CASCADE_MODE | ICW4__NEEDED)
#define PIC2_ICW3       (I_AM_SLAVE_2)
#define PIC2_ICW4       (SNF_MODE_DIS | NONBUFD_MODE | NRML_EOI_MOD \
                         | I8086_EMM_MOD)

#endif /* I8259_REGS_H_ */
