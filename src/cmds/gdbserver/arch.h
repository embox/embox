/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#ifndef ARCH_H_
#define ARCH_H_

#include <stdint.h>

#if defined(__arm__) && !defined(__thumb2__)
#define BREAK_INSTR_SZ 4
#define FEATURE_STR                                             \
	"l<target version=\"1.0\"><architecture>arm</architecture>" \
	"<feature name=\"org.gnu.gdb.arm.core\">"                   \
	"<reg name=\"r0\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name =\"r1\" bitsize=\"32\" type=\"uint32\"/>"        \
	"<reg name=\"r2\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r3\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r4\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r5\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r6\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r7\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r8\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r9\" bitsize=\"32\" type=\"uint32\"/>"         \
	"<reg name=\"r10\" bitsize=\"32\" type=\"uint32\"/>"        \
	"<reg name=\"r11\" bitsize=\"32\" type=\"uint32\"/>"        \
	"<reg name=\"r12\" bitsize=\"32\" type=\"uint32\"/>"        \
	"<reg name=\"sp\" bitsize=\"32\" type=\"data_ptr\"/>"       \
	"<reg name=\"lr\" bitsize=\"32\"/>"                         \
	"<reg name=\"pc\" bitsize=\"32\" type=\"code_ptr\"/>"       \
	"<reg name=\"cpsr\" bitsize=\"32\" regnum=\"25\"/> </feature></target>"
#define REG_LIST_SZ (17 * 4)
#define PC_OFFSET   (15 * 4)
#endif

extern uint8_t break_instr[BREAK_INSTR_SZ];

#endif /* ARCH_H_ */
