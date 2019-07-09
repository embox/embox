/**
 * @file qspi_loader.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.06.2019
 */

#include <assert.h>

#include <lib/tftp.h>

#if defined STM32F746xx
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_qspi.h"
#elif defined STM32F769xx
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_qspi.h"
#else
#error Unsupported platform
#endif

static int qspi_recv_file(char *filename, char *hostname) {
	struct tftp_stream *s = tftp_new_stream(hostname, filename, TFTP_DIR_GET, true);
	int addr = 0;
	int last_block = -1;
	int bytes;
	uint8_t buf[TFTP_SEGSIZE];
	QSPI_Info info;

	BSP_QSPI_Init();

	BSP_QSPI_GetInfo(&info);

	while (1) {
		bytes = tftp_stream_read(s, buf);

		if (bytes < 0) {
			fprintf(stderr, "%s: error: code=%d, msg='%s`\n",
					hostname, -bytes, tftp_error(s));
			tftp_delete_stream(s);
			return bytes;
		}

		if (bytes == 0) {
			/* End of file */
			break;
		}

		if (last_block != (addr + bytes) / info.EraseSectorSize) {
			do {
				last_block++;
				assert(last_block <= info.EraseSectorsNumber);
				BSP_QSPI_Erase_Block(last_block * info.EraseSectorSize);
			} while (last_block < (addr + bytes) / info.EraseSectorSize);
		}

		BSP_QSPI_Write(buf, addr, bytes);

		addr += bytes;
	}

	tftp_delete_stream(s);

	BSP_QSPI_EnableMemoryMappedMode();
	return 0;
}

int main(int argc, char **argv) {

	if (argc != 3) {
		printf("Usage: %s file_name tftp_srv_addr\n", argv[0]);
	}

	if (qspi_recv_file(argv[argc - 2], argv[argc - 1])) {
		fprintf(stderr, "Error\n");
		return 0;
	}

	return 0;
}
