#include <stdint.h>

#include <drivers/common/memory.h>
#include <unistd.h>
#include <sys/mman.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define PAGE_FILE "/proc/self/pagemap"
#define PAGE_INFO_SIZE 8
#define GET_PFN(val) (val & 0x7FFFFFFFFFFFFFLU)


#define ADC_MAP_ADDR           0xC0001000
#define ADC_MAP_SIZE           (0x400)
#define ADC_MAP_MASK           (ADC_MAP_SIZE-1)
#define SDRAM_MAP_ADDR           0xFFC25000
#define SDRAM_MAP_SIZE           (0x1000)
#define SDRAM_MAP_MASK           (ADC_MAP_SIZE-1)

PERIPH_MEMORY_DEFINE(adc_map, ADC_MAP_ADDR, ADC_MAP_SIZE);
PERIPH_MEMORY_DEFINE(sdram_map, SDRAM_MAP_ADDR, SDRAM_MAP_SIZE);
#define COMMAND_START_DMA 0x01
#define COMMAND_DEBUG 0x02
#define COMMAND_STANDBY 0x03


#define CTL_REG_DMA_MEM_ADDR 0x00
#define CTL_REG_DMA_MEM_SIZE 0x01

#define MON_REG_CURRENT_STATE 0x20
#define MON_REG_DMA_STARTED 0x21
#define MON_REG_MEM_OFFSET 0x22
#define MON_REG_MEM_SIZE 0x23
#define MON_REG_MEM_START 0x24
#define MON_REG_STROBE_BITS 0x25
#define MON_REG_STATE_BITS 0x26
#define MON_REG_COMMAND_ACCEPTED 0x27

#define MAX_STATE 3

static const char *m_stateStrings[MAX_STATE + 1] = {
	"STATE_StandBy",
	"STATE_RunningDMA",
	"STATE_Debug",
	"WRONGSTATE!!!!"
};

static uint8_t *m_mapPageAddr;

static int posix_memalign(void **memptr, size_t alignment, size_t size) {
	*memptr = memalign(alignment, size);
	return 0;
}

static void *allocBuffer(int size) {
	int err;
	int *buf;

	if ((err = posix_memalign((void **) &buf, getpagesize(), size))) {
		printf("posix_memalign failed");
		exit(1);
	}

	memset(buf, 0, size);
	return buf;
}


static void freeBuffer(void *buf, int size) {
	free(buf);
}

static void enableSdRamChannels() {
	uintptr_t m_sdramConfigPage = SDRAM_MAP_ADDR;

	*((uint32_t *) (m_sdramConfigPage + 0x80)) = 0x3FFF;
}

static uint32_t readReg(off_t byte_addr) {
	uint8_t *map_byte_addr = m_mapPageAddr + (byte_addr * 4 & ADC_MAP_MASK);
	uint32_t data = *( ( uint32_t *) map_byte_addr );
	return data;
}

static void writeReg(off_t byte_addr, uint32_t val) {
	uint8_t *map_byte_addr = m_mapPageAddr + (byte_addr * 4 & ADC_MAP_MASK);
	*( ( uint32_t *) map_byte_addr ) = val;
}


static uint32_t readControlReg(off_t byte_addr) {
	return readReg(byte_addr + 0x80);
}

static void writeControlReg(off_t byte_addr, uint32_t val) {
	writeReg(byte_addr + 0x80, val);
}

static void command(uint8_t cmd) {
	writeReg(0xC0 + cmd, 0xFFFFFFFF);
}

void dcache_inval(const void *p, size_t size);
int main() {
	void *m_dmaBuffer = allocBuffer(8 * 4096);

	uint32_t m_dmaPhysAddr = (uintptr_t) m_dmaBuffer;

	m_mapPageAddr = (void *) ADC_MAP_ADDR;

	enableSdRamChannels();

	printf("Page virt. addr: %p\n", m_dmaBuffer);

	printf("Page phys. addr: %p\n", (void *) m_dmaPhysAddr);
	printf("Transmitted to FPGA: 0x%X\n", (uint32_t) (m_dmaPhysAddr / 4));

	writeControlReg(CTL_REG_DMA_MEM_ADDR, (uint32_t) (m_dmaPhysAddr / 4));
	writeControlReg(CTL_REG_DMA_MEM_SIZE, 1024 * 8);
	command(COMMAND_START_DMA);

	usleep(1000000);

	for (int i = 0; i < 1024; i++) {
		if (i % 8 == 0) {
			printf("\n");
		}
		dcache_inval((uint32_t *) m_dmaBuffer, 4);
		printf("%08x ", ((uint32_t *) m_dmaBuffer)[i]);
	}
	printf("\n\n");

	printf("DMA mechanism started: 0x%X\n", readControlReg(MON_REG_DMA_STARTED));
	printf("Current memory offset: 0x%X\n", readControlReg(MON_REG_MEM_OFFSET));
	printf("Size to write: 0x%X\n", readControlReg(MON_REG_MEM_SIZE));
	printf("Memory start (phys. addr):  0x%X\n", readControlReg(MON_REG_MEM_START));

	command(COMMAND_STANDBY);

	int currentState = readControlReg(MON_REG_CURRENT_STATE);
	printf("Current state: %s\n", m_stateStrings[(currentState < MAX_STATE)?currentState:MAX_STATE]);

	freeBuffer(m_dmaBuffer, 8 * 4096);

	return 0;
}
