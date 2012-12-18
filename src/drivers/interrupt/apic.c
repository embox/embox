/**
 * @file
 * @brief
 *
 * @date 18.12.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <types.h>

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_ENABLE 0x800
#define APIC_SPURIOUS_INTERRUPT_VECTOR      0x0F
#define APIC_INTERRUPT_COMMAND_REGISTER     0x30

void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   asm volatile("rdmsr":"=a"(*lo),"=d"(*hi):"c"(msr));
}

void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi)
{
   asm volatile("wrmsr"::"a"(lo),"d"(hi),"c"(msr));
}

void cpuSetAPICBase(uint32_t apic)
{
   uint32_t edx = 0;
   uint32_t eax = (apic & 0xfffff000) | IA32_APIC_BASE_MSR_ENABLE;

   cpuSetMSR(IA32_APIC_BASE_MSR, eax, edx);
}

uint32_t cpuGetAPICBase(void)
{
   uint32_t eax, edx;
   cpuGetMSR(IA32_APIC_BASE_MSR, &eax, &edx);

   return (eax & 0xfffff000);
}

uint32_t readAPICRegister(uint32_t reg) {
    return *((volatile uint32_t *) (cpuGetAPICBase() + reg * 16));
}

void writeAPICRegister(uint32_t reg, uint32_t value) {
    *((volatile uint32_t *) (cpuGetAPICBase() + reg * 16)) = value;
}

void sendSIPI(uint32_t apicId, uint32_t addr) {
    uint32_t high = 0;
    uint32_t low  = 0;

    low = (addr >> 12) & 0xFF;
    low |= 6 << 8;                          // Start up IPI
    //icr.DestinationMode = 0;                // Physical
    //icr.DeliveryStatus = 0;                 // Idle
    //icr.Reserved1 = 0;
    low |= 1 << 14;                            // Assert
    //icr.TriggerMode = 0;                    // Edge
    //icr.Reserved2 = 0;
    //icr.DestinationShorthand = 0;           // No shorthand
    //icr.Reserved3 = 0;
    //icr.Reserved4 = 0;
    high = apicId << 24;// Processor to send SIPI to
    //icr.Destination = apicId;               // Processor to send SIPI to

    writeAPICRegister(APIC_INTERRUPT_COMMAND_REGISTER + 1, high);
    writeAPICRegister(APIC_INTERRUPT_COMMAND_REGISTER, low);
}

void newKernel(void) {
	asm ("hlt");
}

#include <string.h>

void cpuEnableAPIC(void)
{
    /* Hardware enable the Local APIC if it wasn't enabled */
    cpuSetAPICBase(cpuGetAPICBase());

    /* Set the Spourious Interrupt Vector Register bit 8 to start receiving interrupts */
    writeAPICRegister(APIC_SPURIOUS_INTERRUPT_VECTOR, readAPICRegister(APIC_SPURIOUS_INTERRUPT_VECTOR) | 0x100);

    memcpy((void *) 0x2000, (char *)newKernel, 512);
    sendSIPI(1, (uint32_t) 0x2000);
}



EMBOX_UNIT_INIT(unit_init);

static int unit_init(void) {
	static int inited = 0;
	if (1 == inited) {
		return 0;
	}
	inited = 1;

	cpuEnableAPIC();

	/* Initialize the master */
//	out8(PIC1_ICW1, PIC1_COMMAND);
//	out8(PIC1_BASE, PIC1_DATA);
//	out8(PIC1_ICW3, PIC1_DATA);
//	out8(PIC1_ICW4, PIC1_DATA);

//	/* Initialize the slave */
//	out8(PIC2_ICW1, PIC2_COMMAND);
//	out8(PIC2_BASE, PIC2_DATA);
//	out8(PIC2_ICW3, PIC2_DATA);
//	out8(PIC2_ICW4, PIC2_DATA);

//	out8(NON_SPEC_EOI, PIC1_COMMAND);
//	out8(NON_SPEC_EOI, PIC2_COMMAND);

//	out8(PICM_MASK, PIC1_DATA);
//	out8(PICS_MASK, PIC2_DATA);

	return 0;
}

void apic_init(void) {
	unit_init();
}

void irqctrl_enable(unsigned int irq) {

}

void irqctrl_disable(unsigned int irq) {

}

void irqctrl_force(unsigned int irq) {

}

int i8259_irq_pending(unsigned int irq) {
	return 0;
}

/* Sends an EOI (end of interrupt) signal to the PICs. */
void i8259_send_eoi(unsigned int irq) {

}
