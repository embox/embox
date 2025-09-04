/**
 * @file sd_mmc_cmd.h
 * @brief SD/MMC commands and constants
 * @author Ruslan Nafikov <ruslan.nafikov.05@bk.ru>
 * @version
 * @date 04.09.2025 
 */

#ifndef SRC_DRIVERS_MMC_CORE_SD_CMD_H_
#define SRC_DRIVERS_MMC_CORE_SD_CMD_H_

// SD/MMC commands
#define SD_CMD_GO_IDLE_STATE      0
#define SD_CMD_ALL_SEND_CID       2
#define SD_CMD_SEND_RELATIVE_ADDR 3
#define SD_CMD_SEND_IF_COND       8
#define SD_CMD_SEND_CSD           9
#define SD_CMD_SELECT_CARD        7
#define SD_CMD_APP_CMD            55
#define SD_CMD_SD_SEND_OP_COND    41
 
// Constants SD/MMC
#define OCR_HCS           (1u << 30) /* Flag for high-capacity storage.
				     * We assume that HC supports it */
#define OCR_READY         (1u << 31) /* When set to 1, SD card powerup is finished */
#define SD_VOLTAGE_WINDOW 0xFF8000
#define CMD55_VALID_RESP  0x120



#endif /* SRC_DRIVERS_MMC_CORE_SD_CMD_H_ */