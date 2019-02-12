/**
 * @file
 *
 * @date Jul 30, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_MTD_OPENNAND_H_
#define SRC_DRIVERS_MTD_OPENNAND_H_

/*
 * Command Register F220h (R/W)
 */
#define ONENAND_CMD_READ                (0x00)
#define ONENAND_CMD_READOOB             (0x13)
#define ONENAND_CMD_PROG                (0x80)
#define ONENAND_CMD_PROGOOB             (0x1A)
#define ONENAND_CMD_2X_PROG             (0x7D)
#define ONENAND_CMD_2X_CACHE_PROG       (0x7F)
#define ONENAND_CMD_UNLOCK              (0x23)
#define ONENAND_CMD_LOCK                (0x2A)
#define ONENAND_CMD_LOCK_TIGHT          (0x2C)
#define ONENAND_CMD_UNLOCK_ALL          (0x27)
#define ONENAND_CMD_ERASE               (0x94)
#define ONENAND_CMD_MULTIBLOCK_ERASE    (0x95)
#define ONENAND_CMD_ERASE_VERIFY        (0x71)
#define ONENAND_CMD_RESET               (0xF0)
#define ONENAND_CMD_OTP_ACCESS          (0x65)
#define ONENAND_CMD_READID              (0x90)
#define FLEXONENAND_CMD_PI_UPDATE       (0x05)
#define FLEXONENAND_CMD_PI_ACCESS       (0x66)
#define FLEXONENAND_CMD_RECOVER_LSB     (0x05)

#endif /* SRC_DRIVERS_MTD_OPENNAND_H_ */
